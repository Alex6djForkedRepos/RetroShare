/*
 * RetroShare Friend Server
 * Copyright (C) 2021-2021  retroshare team <retroshare.project@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-FileCopyrightText: Retroshare Team <contact@retroshare.cc>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "util/rsnet.h"
#include "util/rsprint.h"
#include "util/rsdebug.h"

#include "pqi/pqithreadstreamer.h"
#include "friend_server/fsbio.h"

#include "network.h"
#include "friend_server/fsitem.h"

FsNetworkInterface::FsNetworkInterface()
    : mFsNiMtx(std::string("FsNetworkInterface"))
{
    RS_STACK_MUTEX(mFsNiMtx);

    mClintListn = 0;

    mClintListn = socket(AF_INET, SOCK_STREAM, 0); // creating socket

    int flags = fcntl(mClintListn, F_GETFL);
    fcntl(mClintListn, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in ipOfServer;
    memset(&ipOfServer, '0', sizeof(ipOfServer));

    ipOfServer.sin_family = AF_INET;
    ipOfServer.sin_port = htons(2017); // this is the port number of running server
    ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(mClintListn, (struct sockaddr*)&ipOfServer , sizeof(ipOfServer)) < 0)
    {
        RsErr() << "Error while binding: errno=" << errno ;
        return;
    }

    if(listen(mClintListn , 40) < 0)
    {
        RsErr() << "Error while calling listen: errno=" << errno ;
        return;
    }

    RsDbg() << "Network interface now listening for TCP on " << sockaddr_storage_tostring( *(sockaddr_storage*)&ipOfServer) ;
}

FsNetworkInterface::~FsNetworkInterface()
{
    for(auto& it:mConnections)
    {
        delete it.second.pqi;
        std::cerr << "Releasing socket " << it.second.socket << std::endl;
        close(it.second.socket);
    }
    std::cerr << "Releasing listening socket " << mClintListn << std::endl;
    close(mClintListn);
}
void FsNetworkInterface::threadTick()
{
    // 1 - check for new connections

    checkForNewConnections();

    // 2 - tick all streamers

    for(auto& it:mConnections)
        it.second.pqi->tick();

    rstime::rs_usleep(1000*200);
}

static RsPeerId makePeerId(int t)
{
    unsigned char s[RsPeerId::SIZE_IN_BYTES];
    *reinterpret_cast<int*>(&s) = t;
    return RsPeerId::fromBufferUnsafe(s);
}
bool FsNetworkInterface::checkForNewConnections()
{
    // look for incoming data

    struct sockaddr addr;
    socklen_t addr_len = sizeof(sockaddr);

    int clintConnt = accept(mClintListn, &addr, &addr_len); // accept is a blocking call!

    if(clintConnt < 0)
    {
        if(errno == EWOULDBLOCK)
            ;//RsErr()<< "Incoming connection with nothing to read!" << std::endl;
        else
            RsErr()<< "Error when accepting connection." << std::endl;

        return false;
    }
    RsDbg() << "Got incoming connection from " << sockaddr_storage_tostring( *(sockaddr_storage*)&addr);

    // Make the socket non blocking so that we can read from it and return if nothing comes

    int flags = fcntl(clintConnt, F_GETFL);
    fcntl(clintConnt, F_SETFL, flags | O_NONBLOCK);

    // Create connection info

    ConnectionData c;
    c.socket = clintConnt;
    c.client_address = addr;

    RsPeerId pid = makePeerId(clintConnt);

    // Setup a pqistreamer to deserialize whatever comes from this connection

    RsSerialiser *rss = new RsSerialiser ;
    rss->addSerialType(new FsSerializer) ;

    FsBioInterface *bio = new FsBioInterface(clintConnt);

    auto p = new pqistreamer(rss, pid, bio,BIN_FLAGS_READABLE | BIN_FLAGS_WRITEABLE);
    auto pqi = new pqithreadstreamer(p,rss, pid, bio,BIN_FLAGS_READABLE | BIN_FLAGS_WRITEABLE);
    c.pqi = pqi;

    pqi->start();

    RS_STACK_MUTEX(mFsNiMtx);
    mConnections[makePeerId(clintConnt)] = c;

    return true;
}

RsItem *FsNetworkInterface::GetItem()
{
    RS_STACK_MUTEX(mFsNiMtx);

    for(auto& it:mConnections)
    {
        RsItem *item = it.second.pqi->GetItem();
        if(item)
            return item;
    }
    return nullptr;
}

