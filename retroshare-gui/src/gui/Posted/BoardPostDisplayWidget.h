/*******************************************************************************
 * retroshare-gui/src/gui/Posted/BoardPostDisplayWidget.h                      *
 *                                                                             *
 * Copyright (C) 2019 by Retroshare Team       <retroshare.project@gmail.com>  *
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

#include <QMetaType>
#include <QWidget>

#include <retroshare/rsposted.h>

namespace Ui {
class BoardPostDisplayWidget;
}

class RsPostedPost;

class BoardPostDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	BoardPostDisplayWidget(const RsPostedPost& post,QWidget *parent=nullptr);
	virtual ~BoardPostDisplayWidget();

	static const char *DEFAULT_BOARD_IMAGE;
protected:
	/* GxsGroupFeedItem */

	void setup() ;
	void fill() ;
    void doExpand(bool open) {}
	void setComment(const RsGxsComment&) ;
	void setReadStatus(bool isNew, bool isUnread) ;
    void toggle() {}
	void setCommentsSize(int comNb) ;
    void makeUpVote() ;
    void makeDownVote() ;
	void toggleNotes() ;

signals:
	void vote(const RsGxsGrpMsgIdPair& msgId, bool up_or_down);

private:
	/** Qt Designer generated object */
	Ui::BoardPostDisplayWidget *ui;

	RsPostedPost mPost;
};
