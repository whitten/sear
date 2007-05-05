// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SDL_loadso.h"

#include "src/Bonjour.h"
#include "src/Metaserver.h"

#include <string>
#include <stdio.h>

#include <errno.h>  
#include <unistd.h>   
#include <vector>
#include <cassert>

#ifdef HAVE_BONJOUR
#define _MSL_STDINT_H
#include <inttypes.h>
#include "dns_sd.h"
#include <winsock2.h>

//  Default search strings
static const std::string STR_service = "_worldforge._tcp";  
static const std::string STR_domain  = "local";

// Declare funtion pointers

typedef int (DNSSD_API *pfnDNSServiceRefSockFD)(DNSServiceRef sdRef);

typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceProcessResult) (DNSServiceRef sdRef);

typedef void (DNSSD_API *pfnDNSServiceRefDeallocate) (DNSServiceRef sdRef);

typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceBrowse)
    (
    DNSServiceRef                       *,
    DNSServiceFlags                     ,
    uint32_t                            ,
    const char                          *,
    const char                          *,   /* may be NULL */
    DNSServiceBrowseReply               ,
    void                                *    /* may be NULL */
    );

typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceResolve)
    (
    DNSServiceRef                       *,
    DNSServiceFlags                     ,
    uint32_t                            ,
    const char                          *,
    const char                          *,
    const char                          *,
    DNSServiceResolveReply              ,
    void                                *  /* may be NULL */
    );

typedef DNSServiceErrorType (DNSSD_API *pfnDNSServiceQueryRecord)
    (
    DNSServiceRef                       *,
    DNSServiceFlags                     ,
    uint32_t                            ,
    const char                          *,
    uint16_t                            ,
    uint16_t                            ,
    DNSServiceQueryRecordReply          ,
    void                                *  /* may be NULL */
    );

pfnDNSServiceRefSockFD SEAR_DNSServiceRefSockFD = 0;
pfnDNSServiceQueryRecord SEAR_DNSServiceQueryRecord = 0;
pfnDNSServiceResolve SEAR_DNSServiceResolve = 0;
pfnDNSServiceBrowse SEAR_DNSServiceBrowse = 0;
pfnDNSServiceProcessResult SEAR_DNSServiceProcessResult = 0;
pfnDNSServiceRefDeallocate SEAR_DNSServiceRefDeallocate = 0;


typedef std::vector<std::pair<DNSServiceRef,bool> > ResolverList;
#endif

// Data strcture passed about
class BonjourUserData  {
public:
#ifdef HAVE_BONJOUR

  BonjourUserData():
    meta(0)
  {}
  ~BonjourUserData() {
    // Clean up if required
    if (client) SEAR_DNSServiceRefDeallocate(client);
    while (resolvers.empty() == false) {
      DNSServiceRef c = resolvers.begin()->first;
      if (c) SEAR_DNSServiceRefDeallocate(c);
      resolvers.erase(resolvers.begin());
    }
  }

  ResolverList resolvers;
  Sear::Metaserver *meta;
  DNSServiceRef client;
#endif
};

#ifdef HAVE_BONJOUR
static void DNSSD_API resolve_callback(DNSServiceRef client, const DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorCode,
        const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const unsigned char *txtRecord, void *context)
        {
  assert(context);
  BonjourUserData *ud = reinterpret_cast<BonjourUserData*>(context);

  printf("resolve_callback called -- %s %s %hu\n" , fullname, hosttarget, ntohs(opaqueport));
//printf("%u: %s\n", txtLen, txtRecord);
  // Hopefully this will never be a problem like on linux where the .local hostnames may not be resolved!
  // TODO: This should be backgrounded as it is a blocking request.
  hostent *he = gethostbyname(hosttarget);

 if (he != 0) {
    // If gethostbyname succeeded, we now have an address
    sockaddr_in out_peer;
    ::memcpy(&((sockaddr_in&)out_peer).sin_addr, he->h_addr_list[0],
                                                 he->h_length);

    printf("IP: %s \n", inet_ntoa(out_peer.sin_addr));
    Sear::ServerObject so;
    so.hostname = inet_ntoa(out_peer.sin_addr);
    so.servername = fullname;
    so.port = ntohs(opaqueport);
    so.ping = -1;
    so.num_clients = -1;
    so.uptime = -1.0;
    ud->meta->addServerObject(so);
  } else  {
    printf("gethostbyname failed\n");
  }


// TODO: We need to be able to deallocate this object.
// Can't do it here yet as this will invalidate our iterators we are polling through.
// Perhaps we should be using a map here?
//    DNSServiceRefDeallocate(client);

  ResolverList::iterator I = ud->resolvers.begin();
  ResolverList::const_iterator Iend = ud->resolvers.end();
  while (I != Iend) {
   if (I->first == client) {
     I->second = true;
     break;
   }
    ++I;
  }
}


static void DNSSD_API browse_callback(DNSServiceRef client, DNSServiceFlags flags, uint32_t ifIndex, DNSServiceErrorType errorcode, const char *replyName, const char *replyType, const char *replyDomain, void *context) {
  printf("browse_callback called -- %s\n", replyName);

  assert(context);
  BonjourUserData *ud = reinterpret_cast<BonjourUserData*>(context);
  uint32_t opinterface = kDNSServiceInterfaceIndexAny;

  // Create a new entry by expanding list
  int idx = ud->resolvers.size();
  ud->resolvers.resize(ud->resolvers.size() + 1);
  // Hook up callback to reolve this entry
  SEAR_DNSServiceResolve(&(ud->resolvers[idx].first), 0, opinterface, replyName, replyType, replyDomain, (DNSServiceResolveReply)resolve_callback, context);

}

#endif

namespace Sear {

Bonjour::Bonjour() :
  m_initialised(false),
  m_hnd(0)
{}

Bonjour::~Bonjour() {
  if (m_initialised) shutdown();
} 

void Bonjour::poll() {
  assert(m_initialised == true);
#ifdef HAVE_BONJOUR

  DNSServiceErrorType err;
  // Poll for events
  fd_set  readfds;
  struct timeval tv;

  int fd = SEAR_DNSServiceRefSockFD(m_ud->client);

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);

  // Hook in the resolver sockets
  int nfds = fd + 1;
  ResolverList::const_iterator I = m_ud->resolvers.begin();
  ResolverList::const_iterator Iend = m_ud->resolvers.end();
  while (I != Iend) {
    int fd_2 =  (I->first) ? SEAR_DNSServiceRefSockFD(I->first) : -1;
    if (fd_2 >= nfds) nfds = fd_2 + 1;
    FD_SET(fd_2, &readfds);
    ++I;
  }

  // Change values?
  tv.tv_sec = 0;
  tv.tv_usec = 10;

  int result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
  if (result > 0) { // There is some data!
    if (FD_ISSET(fd, &readfds)) {
      err = SEAR_DNSServiceProcessResult(m_ud->client);
      if (err) {
        printf("Err  %d\n", err);
        return;
      }
    }

    ResolverList::const_iterator I = m_ud->resolvers.begin();
    ResolverList::const_iterator Iend = m_ud->resolvers.end();
    while (I != Iend) {
      int fd_2 =  (I->first) ? SEAR_DNSServiceRefSockFD(I->first) : -1;
      if ( FD_ISSET(fd_2, &readfds)) {
        err = SEAR_DNSServiceProcessResult(I->first);
        if (err) {
          printf("Err  %d\n", err);
          return;
        }
      }
      ++I;
    }
  } else if (result == 0) {
     // Nothing to process
  } else {
    printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
    if (errno != EINTR) {
      printf("EINTR\n");
      return;
    } 
  }

  // Clean up finished resolvers
  ResolverList::iterator J = m_ud->resolvers.begin();
  ResolverList::const_iterator Jend = m_ud->resolvers.end();
  while (J != Jend) {
    if (J->second) {
      SEAR_DNSServiceRefDeallocate(J->first);
      m_ud->resolvers.erase(J);
      // Reset iterators
      J = m_ud->resolvers.begin();
      Jend = m_ud->resolvers.end();
    } else {
      ++J;
    }
  }
#endif
}

int Bonjour::init(Metaserver *meta) {
  assert(m_initialised == false); 
#ifdef HAVE_BONJOUR
  m_hnd = SDL_LoadObject("dnssd.dll");
  if (m_hnd == 0) return 1;

  SEAR_DNSServiceRefSockFD = (pfnDNSServiceRefSockFD)SDL_LoadFunction(m_hnd, "DNSServiceRefSockFD");
  SEAR_DNSServiceQueryRecord = (pfnDNSServiceQueryRecord)SDL_LoadFunction(m_hnd, "DNSServiceQueryRecord");
  SEAR_DNSServiceResolve = (pfnDNSServiceResolve)SDL_LoadFunction(m_hnd, "DNSServiceResolve");
  SEAR_DNSServiceBrowse = (pfnDNSServiceBrowse)SDL_LoadFunction(m_hnd, "DNSServiceBrowse");
  SEAR_DNSServiceProcessResult = (pfnDNSServiceProcessResult)SDL_LoadFunction(m_hnd, "DNSServiceProcessResult");
  SEAR_DNSServiceRefDeallocate = (pfnDNSServiceRefDeallocate)SDL_LoadFunction(m_hnd, "DNSServiceRefDeallocate");

  m_ud = std::auto_ptr<BonjourUserData>(new BonjourUserData());

  m_ud->meta = meta;

  DNSServiceFlags f = 0;
  DNSServiceErrorType err = SEAR_DNSServiceBrowse(&m_ud->client, f, kDNSServiceInterfaceIndexAny, STR_service.c_str(), STR_domain.c_str(), browse_callback, (void*)m_ud.get());

  if (!m_ud->client || err != 0) {
    m_ud.release();
    return 1;
  }
#endif

  m_initialised = true;

  return 0;
}

void Bonjour::shutdown() {
  assert(m_initialised == true);
#ifdef HAVE_BONJOUR
  m_ud.release();

  SDL_UnloadObject(m_hnd);
  m_hnd = 0;
#endif
  m_initialised = false;
}


} // namespace Sear
