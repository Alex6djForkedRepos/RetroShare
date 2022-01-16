/*******************************************************************************
 * gui/NetworkView.h                                                           *
 *                                                                             *
 * Copyright (c) 2008 Robert Fernie    <retroshare.project@gmail.com>          *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Affero General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Affero General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Affero General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/

#pragma once

#include <QGraphicsScene>

#include "ui_FriendServerControl.h"

class FriendServerControl : public QWidget, public Ui::FriendServerControl
{
    Q_OBJECT

public:
    FriendServerControl(QWidget *parent = 0);
    virtual ~FriendServerControl();

protected slots:
    void onOnOffClick(bool b);
    void onOnionAddressEdit(const QString&);
    void onOnionPortEdit(int);
    void onNbFriendsToRequestsChanged(int n);
    void updateTorProxyInfo();
    void checkServerAddress();

private:
    void updateFriendServerStatusIcon(bool ok);

    QTimer *mConnectionCheckTimer;
    QMovie *mCheckingServerMovie;
};
