// Copyright (c) 2007-2009 Google Inc.
// Copyright (c) 2006-2007 Jaiku Ltd.
// Copyright (c) 2002-2006 Mika Raento and Renaud Petit
//
// This software is licensed at your choice under either 1 or 2 below.
//
// 1. MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// 2. Gnu General Public license 2.0
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//
// This file is part of the JaikuEngine mobile client.

#include "ccn_everybodysfeedview.h"

#include "ccn_commentsview.h"
#include "ccn_feedgroupview.h"
#include <contextcontacts.rsg>
#include "contextcontacts.hrh"

#include "app_context_impl.h"
#include "break.h"
#include "symbian_auto_ptr.h"
#include "reporting.h"

#include "ccu_feedlist.h"
#include "ccu_streamfactories.h"

#include "ccu_mainbgcontainer.h"
#include "ccu_feedmodel.h"
#include "ccu_posterui.h"
#include "ccu_feedfoundation.h"
#include "ccu_activestate.h"
#include "ccu_utils.h"
#include "cu_common.h"
#include "juik_keycodes.h"


#include <akndef.h>
#include <aknsbasicbackgroundcontrolcontext.h> 
#include <aknsdrawutils.h> 
#include <aknviewappui.h>



class CEverybodysFeedViewImpl :
	public CEverybodysFeedView
{
public:
	CEverybodysFeedViewImpl(CCommentsView& aCommentsView) : iCommentsView( aCommentsView )
	{
	}
	

	~CEverybodysFeedViewImpl() 
	{
		CC_TRAPD(err, ReleaseViewImpl());
		if (err!=KErrNone) User::Panic(_L("UNEXPECTED_LEAVE"), err);
	}
	
	void ReleaseViewImpl()
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("ReleaseViewImpl"));
		RemoveContainerL();
	}


	void ConstructL()
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("ConstructL"));
		BaseConstructL( R_YOURCONTACTSSTREAM_VIEW );
	}

	TUid Id() const {
		return KEverybodysFeedView;
	}


	void RealDoActivateL(const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,const TDesC8& /*aCustomMessage*/)
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("RealDoActivateL"));
		iCommentsView.SetParentViewL( ViewId() );
		ActiveState().ActiveContact().ClearL();
		CreateContainerL();
  	StatusPaneUtils::SetTitlePaneTextL(_L("Jaiku"));
	}
			
	void RealDoDeactivateL()
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("RealDoDeactivate"));
		RemoveContainerL();
	}
	

	void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("DynInitMenuPaneL"));

		TBool failedToConstruct = ! iList;
		CommonMenus().DynInitMenuPaneL(aResourceId, aMenuPane, failedToConstruct);
		if ( failedToConstruct )
			return;
		
		refcounted_ptr<CBBFeedItem> item( ActiveState().ActiveItem().GetL() );
		TBool itemFocused = item.get() != NULL;
		SetItemDimmedIfExists(aMenuPane, EContextContactsCmdPostComment, !itemFocused );
	}


	void HandleCommandL(TInt aCommand)
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("HandleCommandL"));
		switch ( aCommand )
			{
			case EAknSoftkeyBack:
				ActivateParentViewL();
				break;
			case EContextContactsCmdSetUserDesc:
				PosterUi().PostJaikuL( this );
				if ( iList )
					iList->SetCurrentItemToIdL(KPostJaikuItem);
				break;
			case EContextContactsCmdMarkAllAsRead:
				{
					FeedStorage().MarkAllAsRead();
				}
				break;
			case EContextContactsCmdPostComment:
				{
					refcounted_ptr<CBBFeedItem> item( ActiveState().ActiveItem().GetL() );
					if (item.get())
						{
							PosterUi().PostCommentL( *item, this );
							if ( iList )
								iList->SetCurrentItemToIdL(KPostJaikuItem);				
						}
					else 
						User::Leave(KErrNotFound);
				}
				break;

			default:
				AppUi()->HandleCommandL( aCommand );
				break;
			}
	}

	class CListEventHandler : public CFeedList::COpenItemOnClick
	{
	public:
		CListEventHandler(class CActiveState& aActiveState,class CAknView& aView) : 
			CFeedList::COpenItemOnClick(aActiveState, aView) {}
		
		virtual TBool ItemClickedL(TInt aIndex, CFeedControl* aControl)
		{
			if (aControl && aControl->Id() == KPostJaikuItem)
				{
					iView.HandleCommandL( EContextContactsCmdSetUserDesc );
					return ETrue;
				}
			return CFeedList::COpenItemOnClick::ItemClickedL( aIndex, aControl );
		}
	};

	void CreateContainerL()
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("CreateContainerL"));
		RemoveContainerL();
		iContainer = CMainBgContainer::NewL( this, ClientRect(), ThemeColors(), ProgressBarModel() );

		if ( ! iFeedModel )
			iFeedModel = CreateEverybodysFeedModelL( FeedStorage() );

		if ( ! iControlFactory )
			iControlFactory = StreamFactories::EverybodysStreamL( UiDelegates(), FeedUi::EShowBuddyIcon | FeedUi::EShowParentInfo );
		
		if ( ! iListEventHandler)
			iListEventHandler = new (ELeave) CListEventHandler( ActiveState(), *this );

		iList = CFeedList::NewL( *this, iContainer, *iFeedModel, *iControlFactory, ActiveState(), iListEventHandler);

		TUint id = ActiveState().ActiveItem().GetId();
		if ( id != KNullItemId )
			iList->SetCurrentItemToIdL( id );

		iContainer->SetContentL( iList );

		iContainer->MakeVisible(ETrue);
		iContainer->ActivateL();

		AppUi()->AddToStackL( *this, iContainer );
	}

	void RemoveContainerL()
	{
		CALLSTACKITEM_N(_CL("CEverybodysFeedViewImpl"), _CL("RemoveContainerL"));
		if ( iContainer )
			{
				AppUi()->RemoveFromStack( iContainer );
				delete iContainer;
				iContainer = NULL;
			}
		if ( iList )
			{
				delete iList;
				iList = NULL;
			}
		if ( iControlFactory)
			{

				delete iControlFactory;
				iControlFactory = NULL;
			}

		if ( iFeedModel )
			{
				delete iFeedModel;
				iFeedModel = NULL;
			}
		if ( iListEventHandler )
			{
				delete iListEventHandler;
				iListEventHandler = NULL;
			}
	}
	
	void HandleResourceChange( TInt aType )
	{
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{
			if ( iContainer )
				{
					TRect r = ClientRect();
					iContainer->SetRect( r );
				}
		}
	}

private:
	CCommentsView& iCommentsView;

	CMainBgContainer* iContainer;
	CFeedList* iList;
	MFeedModel* iFeedModel;
	CFeedList::MControlFactory* iControlFactory;
	CListEventHandler* iListEventHandler;

};


EXPORT_C CEverybodysFeedView* CEverybodysFeedView::NewL(CCommentsView& aCommentsView)
{
	CALLSTACKITEMSTATIC_N(_CL("CEverybodysFeedView"), _CL("NewL"));
	auto_ptr<CEverybodysFeedViewImpl> self(new (ELeave) CEverybodysFeedViewImpl(aCommentsView));
	//self->ConstructL();
	return self.release();
}
