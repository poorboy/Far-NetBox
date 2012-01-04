/**************************************************************************
 *  Far plugin helper (for FAR 2.0) (http://code.google.com/p/farplugs)   *
 *  Copyright (C) 2000-2011 by Artem Senichev <artemsen@gmail.com>        *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#pragma once

#include "boostdefines.hpp"
#include <boost/signals/signal1.hpp>
#include <boost/signals/signal2.hpp>
#include <boost/signals/signal3.hpp>
#include <boost/signals/signal4.hpp>

#include "FarPlugin.h"
#include "stdafx.h"

#define MAX_SIZE -1


//---------------------------------------------------------------------------

class TFarDialogContainer;
class TFarDialogItem;
class TFarButton;
class TFarSeparator;
class TFarBox;
class TFarList;
struct FarDialogItem;
enum TItemPosition { ipNewLine, ipBelow, ipRight };
//---------------------------------------------------------------------------
// typedef void (TObject::*TFarKeyEvent)
    // (TFarDialog *Sender, TFarDialogItem *Item, long KeyCode, bool &Handled);
typedef boost::signal4<void, TFarDialog *, TFarDialogItem *, long, bool &> key_signal_type;
typedef key_signal_type::slot_type key_slot_type;

// typedef void (TObject::*TFarMouseClickEvent) 
    // (TFarDialogItem *Item, MOUSE_EVENT_RECORD *Event);
typedef boost::signal2<void, TFarDialogItem *, MOUSE_EVENT_RECORD *> mouse_click_signal_type;
typedef mouse_click_signal_type::slot_type mouse_click_slot_type;

// typedef void (TObject::*TFarProcessGroupEvent)(TFarDialogItem *Item, void *Arg);
typedef boost::signal2<void, TFarDialogItem *, void *> processgroupevent_signal_type;
typedef processgroupevent_signal_type::slot_type processgroupevent_slot_type;
//---------------------------------------------------------------------------
class TFarDialog : public TObject
{
    friend TFarDialogItem;
    friend TFarDialogContainer;
    friend TFarButton;
    friend TFarList;
    friend class TFarListBox;
    typedef TFarDialog self;
public:
    explicit TFarDialog(TCustomFarPlugin *AFarPlugin);
    virtual ~TFarDialog();

    int ShowModal();
    void ShowGroup(int Group, bool Show);
    void EnableGroup(int Group, bool Enable);

    TRect GetBounds() const { return FBounds; }
    void SetBounds(const TRect &value);
    TRect GetClientRect() const;
    std::wstring GetHelpTopic() const { return FHelpTopic; }
    void SetHelpTopic(const std::wstring &value);
    unsigned int GetFlags() const { return FFlags; }
    void SetFlags(const unsigned int &value);
    bool GetCentered() const;
    void SetCentered(const bool &value);
    TPoint GetSize() const;
    void SetSize(const TPoint &value);
    TPoint GetClientSize() const;
    int GetWidth() const;
    void SetWidth(const int &value);
    size_t GetHeight() const;
    void SetHeight(const size_t &value);
    std::wstring GetCaption() const;
    void SetCaption(const std::wstring &value);
    HANDLE GetHandle() const { return FHandle; }
    TFarButton *GetDefaultButton() const { return FDefaultButton; }
    TFarBox *GetBorderBox() const { return FBorderBox; }
    TFarDialogItem *GetItem(size_t Index);
    size_t GetItemCount() const;
    TItemPosition GetNextItemPosition() const { return FNextItemPosition; }
    void SetNextItemPosition(const TItemPosition &value) { FNextItemPosition = value; }
    void GetNextItemPosition(int &Left, int &Top);
    int GetDefaultGroup() const { return FDefaultGroup; }
    void SetDefaultGroup(const int &value) { FDefaultGroup = value; }
    int GetTag() const { return FTag; }
    void SetTag(const int &value) { FTag = value; }
    TFarDialogItem *GetItemFocused() const { return FItemFocused; }
    void SetItemFocused(TFarDialogItem * const &value);
    int GetResult() const { return FResult; }
    TPoint GetMaxSize();
    
    const key_signal_type &GetOnKey() const { return FOnKey; }
    void SetOnKey(const key_slot_type &value) { FOnKey.connect(value); }

    void Redraw();
    void LockChanges();
    void UnlockChanges();
    int GetSystemColor(unsigned int Index);
    bool HotKey(unsigned long Key);

    TCustomFarPlugin *GetFarPlugin() { return FFarPlugin; }

protected:
    TObjectList *GetItems() { return FItems; }

    void Add(TFarDialogItem *Item);
    void Add(TFarDialogContainer *Container);
    long SendMessage(int Msg, int Param1, int Param2);
    virtual long DialogProc(int Msg, int Param1, long Param2);
    virtual long FailDialogProc(int Msg, int Param1, long Param2);
    long DefaultDialogProc(int Msg, int Param1, long Param2);
    virtual bool MouseEvent(MOUSE_EVENT_RECORD *Event);
    virtual bool Key(TFarDialogItem *Item, long KeyCode);
    virtual void Change();
    virtual void Init();
    virtual bool CloseQuery();
    std::wstring GetMsg(int MsgId);
    void RefreshBounds();
    virtual void Idle();
    void BreakSynchronize();
    void Synchronize(const threadmethod_slot_type &slot);
    void Close(TFarButton *Button);
    void ProcessGroup(int Group, const processgroupevent_slot_type &Callback, void *Arg);
    void ShowItem(TFarDialogItem *Item, void *Arg);
    void EnableItem(TFarDialogItem *Item, void *Arg);
    bool ChangesLocked();
    TFarDialogItem *ItemAt(int X, int Y);

    static LONG_PTR WINAPI DialogProcGeneral(HANDLE Handle, int Msg, int Param1, LONG_PTR Param2);

private:
    TCustomFarPlugin *FFarPlugin;
    TRect FBounds;
    unsigned int FFlags;
    std::wstring FHelpTopic;
    bool FVisible;
    TObjectList *FItems;
    TObjectList *FContainers;
    HANDLE FHandle;
    TFarButton *FDefaultButton;
    TFarBox *FBorderBox;
    TItemPosition FNextItemPosition;
    int FDefaultGroup;
    int FTag;
    TFarDialogItem *FItemFocused;
    key_signal_type FOnKey;
    FarDialogItem *FDialogItems;
    int FDialogItemsCapacity;
    int FChangesLocked;
    bool FChangesPending;
    int FResult;
    bool FNeedsSynchronize;
    HANDLE FSynchronizeObjects[2];
    threadmethod_signal_type FSynchronizeMethod;
    TFarDialog *Self;
};
//---------------------------------------------------------------------------
class TFarDialogContainer : public TObject
{
    friend TFarDialog;
    friend TFarDialogItem;
    typedef TFarDialogContainer self;
public:
    int GetLeft() { return FLeft; }
    void SetLeft(int value) { SetPosition(0, value); }
    int GetTop() { return FTop; }
    void SetTop(int value) { SetPosition(1, value); }
    size_t GetItemCount() const;
    bool GetEnabled() { return FEnabled; }
    void SetEnabled(bool value);

protected:
    explicit TFarDialogContainer(TFarDialog *ADialog);
    virtual ~TFarDialogContainer();

    TFarDialog *GetDialog() { return FDialog; }

    void Add(TFarDialogItem *Item);
    void Remove(TFarDialogItem *Item);
    virtual void Change();
    std::wstring GetMsg(int MsgId);

private:
    int FLeft;
    int FTop;
    TObjectList *FItems;
    TFarDialog *FDialog;
    bool FEnabled;

    void SetPosition(int Index, int value);
};
//---------------------------------------------------------------------------
#define DIF_INVERSE 0x00000001UL
//---------------------------------------------------------------------------
class TFarDialogItem : public TObject
{
    friend TFarDialog;
    friend TFarDialogContainer;
    friend TFarList;
public:
    TRect GetBounds() const { return FBounds; }
    void SetBounds(TRect value);
    TRect GetActualBounds();
    int GetLeft() { return GetCoordinate(0); }
    void SetLeft(int value) { SetCoordinate(0, value); }
    int GetTop() { return GetCoordinate(1); }
    void SetTop(int value) { SetCoordinate(1, value); }
    int GetRight() { return GetCoordinate(2); }
    void SetRight(int value) { SetCoordinate(2, value); }
    int GetBottom() { return GetCoordinate(3); }
    void SetBottom(int value) { SetCoordinate(3, value); }
    int GetWidth();
    void SetWidth(int value);
    size_t GetHeight();
    void SetHeight(int value);
    unsigned int GetFlags();
    void SetFlags(unsigned int value);
    bool GetEnabled() { return FEnabled; }
    void SetEnabled(bool value);
    bool GetIsEnabled() { return FIsEnabled; }
    TFarDialogItem *GetEnabledFollow() { return FEnabledFollow; }
    void SetEnabledFollow(TFarDialogItem *value);
    TFarDialogItem *GetEnabledDependency() { return FEnabledDependency; }
    void SetEnabledDependency(TFarDialogItem *value);
    TFarDialogItem *GetEnabledDependencyNegative() { return FEnabledDependencyNegative; }
    void SetEnabledDependencyNegative(TFarDialogItem *value);
    virtual bool GetIsEmpty();
    int GetGroup() { return FGroup; }
    void SetGroup(int value) { FGroup = value; }
    bool GetVisible() { return GetFlag(DIF_HIDDEN | DIF_INVERSE); }
    void SetVisible(bool value) { SetFlag(DIF_HIDDEN | DIF_INVERSE, value); }
    bool GetTabStop() { return GetFlag(DIF_NOFOCUS | DIF_INVERSE); }
    void SetTabStop(bool value) { SetFlag(DIF_NOFOCUS | DIF_INVERSE, value); }
    bool GetOem() { return FOem; }
    void SetOem(bool value) { FOem = value; }
    int GetTag() { return FTag; }
    void SetTag(int value) { FTag = value; }
    TFarDialog *GetDialog() { return FDialog; }

    const notify_signal_type &GetOnExit() const { return FOnExit; }
    void SetOnExit(const notify_slot_type &value) { FOnExit.connect(value); }
    const mouse_click_signal_type &GetOnMouseClick() const { return FOnMouseClick; }
    void SetOnMouseClick(const mouse_click_slot_type &value) { FOnMouseClick.connect(value); }

    void Move(int DeltaX, int DeltaY);
    void MoveAt(int X, int Y);
    virtual bool CanFocus();
    bool Focused();
    void SetFocus();
    size_t GetItem() { return FItem; }
    void SetItem(size_t value) { FItem = value; }

protected:
    int FDefaultType;
    int FGroup;
    int FTag;
    notify_signal_type FOnExit;
    mouse_click_signal_type FOnMouseClick;

    explicit TFarDialogItem(TFarDialog *ADialog, int AType);
    virtual ~TFarDialogItem();

    FarDialogItem *GetDialogItem();
    bool GetCenterGroup() { return GetFlag(DIF_CENTERGROUP); }
    void SetCenterGroup(bool value) { SetFlag(DIF_CENTERGROUP, value); }
    virtual std::wstring GetData();
    virtual void SetData(const std::wstring &value);
    int GetType();
    void SetType(int value);
    int GetSelected();
    void SetSelected(int value);
    TFarDialogContainer *GetContainer() { return FContainer; }
    void SetContainer(TFarDialogContainer *value);
    bool GetChecked();
    void SetChecked(bool value);

    virtual void Detach();
    void DialogResized();
    long SendMessage(int Msg, int Param);
    long SendDialogMessage(int Msg, int Param1, int Param2);
    virtual long ItemProc(int Msg, long Param);
    long DefaultItemProc(int Msg, int Param);
    long DefaultDialogProc(int Msg, int Param1, int Param2);
    virtual long FailItemProc(int Msg, long Param);
    virtual void Change();
    void DialogChange();
    void SetAlterType(int Index, bool value);
    bool GetAlterType(int Index);
    virtual void UpdateBounds();
    virtual void ResetBounds();
    virtual void Init();
    virtual bool CloseQuery();
    virtual bool MouseMove(int X, int Y, MOUSE_EVENT_RECORD *Event);
    virtual bool MouseClick(MOUSE_EVENT_RECORD *Event);
    TPoint MouseClientPosition(MOUSE_EVENT_RECORD *Event);
    void Text(int X, int Y, int Color, std::wstring Str, bool Oem = false);
    void Redraw();
    virtual bool HotKey(char HotKey);

    virtual void SetDataInternal(const std::wstring &value);
    void UpdateData(const std::wstring &value);
    void UpdateSelected(int value);

    bool GetFlag(int Index);
    void SetFlag(int Index, bool value);

    virtual void DoFocus();
    virtual void DoExit();

    char GetColor(int Index);
    void SetColor(int Index, char value);

private:
    TFarDialog *FDialog;
    TRect FBounds;
    TFarDialogItem *FEnabledFollow;
    TFarDialogItem *FEnabledDependency;
    TFarDialogItem *FEnabledDependencyNegative;
    TFarDialogContainer *FContainer;
    size_t FItem;
    bool FEnabled;
    bool FIsEnabled;
    unsigned long FColors;
    unsigned long FColorMask;
    bool FOem;

    void UpdateFlags(unsigned int value);
    void SetCoordinate(int Index, int value);
    int GetCoordinate(int Index);
    TFarDialogItem *GetPrevItem();
    void UpdateFocused(bool value);
    void UpdateEnabled();
};
//---------------------------------------------------------------------------
class TFarBox : public TFarDialogItem
{
public:
    TFarBox(TFarDialog *ADialog);

    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    virtual bool GetDouble() { return GetAlterType(DI_DOUBLEBOX); }
    virtual void SetDouble(bool value) { SetAlterType(DI_DOUBLEBOX, value); }
};
//---------------------------------------------------------------------------
typedef boost::signal2<void, TFarButton *, bool &> button_click_signal_type;
typedef button_click_signal_type::slot_type button_click_slot_type;

enum TFarButtonBrackets { brNone, brTight, brSpace, brNormal };
//---------------------------------------------------------------------------
class TFarButton : public TFarDialogItem
{
public:
    TFarButton(TFarDialog *ADialog);

    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    virtual int GetResult() { return FResult; }
    virtual void SetResult(int value) { FResult = value; }
    bool GetDefault();
    void SetDefault(bool value);
    TFarButtonBrackets GetBrackets() { return FBrackets; }
    void SetBrackets(TFarButtonBrackets value);
    bool GetCenterGroup() { return TFarDialogItem::GetCenterGroup(); }
    void SetCenterGroup(bool value) { TFarDialogItem::SetCenterGroup(value); }
    virtual const button_click_signal_type &GetOnClick() const { return FOnClick; }
    virtual void SetOnClick(const button_click_slot_type &value) { FOnClick.connect(value); }

protected:
    virtual void SetDataInternal(const std::wstring &value);
    virtual std::wstring GetData();
    virtual long ItemProc(int Msg, long Param);
    virtual bool HotKey(char HotKey);

private:
    int FResult;
    button_click_signal_type FOnClick;
    TFarButtonBrackets FBrackets;
};
//---------------------------------------------------------------------------
// typedef void (*TFarAllowChange)(TFarDialogItem *Sender,
        // long NewState, bool &AllowChange);
typedef boost::signal3<void, TFarDialogItem *, long, bool &> farallowchange_signal_type;
typedef farallowchange_signal_type::slot_type farallowchange_slot_type;
//---------------------------------------------------------------------------
class TFarCheckBox : public TFarDialogItem
{
public:
    TFarCheckBox(TFarDialog *ADialog);

    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    bool GetAllowGrayed() { return GetFlag(DIF_3STATE); }
    void SetAllowGrayed(bool value) { SetFlag(DIF_3STATE, value); }
    virtual farallowchange_signal_type &GetOnAllowChange() { return FOnAllowChange; }
    virtual void SetOnAllowChange(const farallowchange_slot_type &value) { FOnAllowChange.connect(value); }
    bool GetChecked() { return TFarDialogItem::GetChecked(); }
    void SetChecked(bool value) { TFarDialogItem::SetChecked(value); }
    int GetSelected() { return TFarDialogItem::GetSelected(); }
    void SetSelected(int value) { TFarDialogItem::SetSelected(value); }

protected:
    farallowchange_signal_type FOnAllowChange;
    virtual long ItemProc(int Msg, long Param);
    virtual bool GetIsEmpty();
    virtual void SetData(const std::wstring &value);
};
//---------------------------------------------------------------------------
class TFarRadioButton : public TFarDialogItem
{
public:
    TFarRadioButton(TFarDialog *ADialog);

    bool GetChecked() { return TFarDialogItem::GetChecked(); }
    void SetChecked(bool value) { TFarDialogItem::SetChecked(value); }
    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    virtual farallowchange_signal_type &GetOnAllowChange() { return FOnAllowChange; }
    virtual void SetOnAllowChange(const farallowchange_slot_type &value) { FOnAllowChange.connect(value); }

protected:
    farallowchange_signal_type FOnAllowChange;
    virtual long ItemProc(int Msg, long Param);
    virtual bool GetIsEmpty();
    virtual void SetData(const std::wstring &value);
};
//---------------------------------------------------------------------------
class TFarEdit : public TFarDialogItem
{
public:
    TFarEdit(TFarDialog *ADialog);

    virtual std::wstring GetText() { return GetData(); }
    virtual void SetText(std::wstring value) { SetData(value); }
    int GetAsInteger();
    void SetAsInteger(int value);
    virtual bool GetPassword() { return GetAlterType(DI_PSWEDIT); }
    virtual void SetPassword(bool value) { SetAlterType(DI_PSWEDIT, value); }
    virtual bool GetFixed() { return GetAlterType(DI_FIXEDIT); }
    virtual void SetFixed(bool value) { SetAlterType(DI_FIXEDIT, value); }

    virtual std::wstring GetMask() { return GetHistoryMask(1); }
    virtual void SetMask(std::wstring value) { SetHistoryMask(1, value); }
    virtual std::wstring GetHistory() { return GetHistoryMask(0); }
    virtual void SetHistory(std::wstring value) { SetHistoryMask(0, value); }
    bool GetExpandEnvVars() { return GetFlag(DIF_EDITEXPAND); }
    void SetExpandEnvVars(bool value) { SetFlag(DIF_EDITEXPAND, value); }
    bool GetAutoSelect() { return GetFlag(DIF_SELECTONENTRY); }
    void SetAutoSelect(bool value) { SetFlag(DIF_SELECTONENTRY, value); }
    bool GetReadOnly() { return GetFlag(DIF_READONLY); }
    void SetReadOnly(bool value) { SetFlag(DIF_READONLY, value); }

protected:
    virtual long ItemProc(int Msg, long Param);
    virtual void Detach();

private:
    std::wstring GetHistoryMask(int Index);
    void SetHistoryMask(int Index, std::wstring value);
};
//---------------------------------------------------------------------------
class TFarSeparator : public TFarDialogItem
{
public:
    TFarSeparator(TFarDialog *ADialog);

    bool GetDouble();
    void SetDouble(bool value);
    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    int GetPosition();
    void SetPosition(int value);

protected:
    virtual void ResetBounds();

private:
};
//---------------------------------------------------------------------------
class TFarText : public TFarDialogItem
{
public:
    TFarText(TFarDialog *ADialog);

    virtual std::wstring GetCaption() { return GetData(); }
    virtual void SetCaption(std::wstring value) { SetData(value); }
    bool GetCenterGroup() { return TFarDialogItem::GetCenterGroup(); }
    void SetCenterGroup(bool value) { TFarDialogItem::SetCenterGroup(value); }
    bool GetColor() { return TFarDialogItem::GetColor(0) != 0; }
    void SetColor(bool value) { TFarDialogItem::SetColor(0, value); }

protected:
    virtual void SetData(const std::wstring &value);
};
//---------------------------------------------------------------------------
class TFarListBox;
class TFarComboBox;
class TFarLister;
//---------------------------------------------------------------------------
class TFarList : public TStringList
{
    friend TFarListBox;
    friend TFarLister;
    friend TFarComboBox;
    typedef TFarList self;
public:
    explicit TFarList(TFarDialogItem *ADialogItem = NULL);
    virtual ~TFarList();

    virtual void Assign(TPersistent *Source);

    int GetSelected();
    void SetSelected(int value);
    int GetTopIndex();
    void SetTopIndex(int value);
    int GetMaxLength();
    int GetVisibleCount();
    unsigned int GetFlags(int Index);
    void SetFlags(int Index, unsigned int value);
    bool GetDisabled(int Index) { return GetFlag(Index, LIF_DISABLE); }
    void SetDisabled(int Index, bool value) { SetFlag(Index, LIF_DISABLE, value); }
    bool GetChecked(int Index) { return GetFlag(Index, LIF_CHECKED); }
    void SetChecked(int Index, bool value) { SetFlag(Index, LIF_CHECKED, value); }

protected:
    virtual void Changed();
    virtual long ItemProc(int Msg, long Param);
    virtual void Init();
    void UpdatePosition(int Position);
    int GetPosition();
    virtual void Put(int Index, const std::wstring &S);
    void SetCurPos(int Position, int TopIndex);
    void UpdateItem(int Index);

    FarList *GetListItems() { return FListItems; }
    // void SetListItems(FarList *value) { FListItems = value; }
    TFarDialogItem *GetDialogItem() { return FDialogItem; }

private:
    FarList *FListItems;
    TFarDialogItem *FDialogItem;
    bool FNoDialogUpdate;
    TFarList *Self;

    inline int GetSelectedInt(bool Init);
    bool GetFlag(int Index, int Flag);
    void SetFlag(int Index, int Flag, bool value);
};
//---------------------------------------------------------------------------
enum TFarListBoxAutoSelect { asOnlyFocus, asAlways, asNever };
//---------------------------------------------------------------------------
class TFarListBox : public TFarDialogItem
{
    typedef TFarListBox self;
public:
    explicit TFarListBox(TFarDialog *ADialog);
    virtual ~TFarListBox();

    void SetItems(TStrings *value);

    bool GetNoAmpersand() { return GetFlag(DIF_LISTNOAMPERSAND); }
    void SetNoAmpersand(bool value) { SetFlag(DIF_LISTNOAMPERSAND, value); }
    bool GetAutoHighlight() { return GetFlag(DIF_LISTAUTOHIGHLIGHT); }
    void SetAutoHighlight(bool value) { SetFlag(DIF_LISTAUTOHIGHLIGHT, value); }
    bool GetNoBox() { return GetFlag(DIF_LISTNOBOX); }
    void SetNoBox(bool value) { SetFlag(DIF_LISTNOBOX, value); }
    bool GetWrapMode() { return GetFlag(DIF_LISTWRAPMODE); }
    void SetWrapMode(bool value) { SetFlag(DIF_LISTWRAPMODE, value); }
    TFarList *GetItems() { return FList; }
    void SetList(TFarList *value);
    TFarListBoxAutoSelect GetAutoSelect() { return FAutoSelect; }
    void SetAutoSelect(TFarListBoxAutoSelect value);

protected:
    virtual long ItemProc(int Msg, long Param);
    virtual void Init();
    virtual bool CloseQuery();

private:
    TFarList *FList;
    TFarListBoxAutoSelect FAutoSelect;
    bool FDenyClose;

    void UpdateMouseReaction();
};
//---------------------------------------------------------------------------
class TFarComboBox : public TFarDialogItem
{
    typedef TFarComboBox self;
public:
    explicit TFarComboBox(TFarDialog *ADialog);
    virtual ~TFarComboBox();

    void ResizeToFitContent();

    bool GetNoAmpersand() { return GetFlag(DIF_LISTNOAMPERSAND); }
    void SetNoAmpersand(bool value) { SetFlag(DIF_LISTNOAMPERSAND, value); }
    bool GetAutoHighlight() { return GetFlag(DIF_LISTAUTOHIGHLIGHT); }
    void SetAutoHighlight(bool value) { SetFlag(DIF_LISTAUTOHIGHLIGHT, value); }
    bool GetWrapMode() { return GetFlag(DIF_LISTWRAPMODE); }
    void SetWrapMode(bool value) { SetFlag(DIF_LISTWRAPMODE, value); }
    TFarList *GetItems() { return FList; }
    virtual std::wstring GetText() { return GetData(); }
    virtual void SetText(std::wstring value) { SetData(value); }
    bool GetAutoSelect() { return GetFlag(DIF_SELECTONENTRY); }
    void SetAutoSelect(bool value) { SetFlag(DIF_SELECTONENTRY, value); }
    bool GetDropDownList() { return GetFlag(DIF_DROPDOWNLIST); }
    void SetDropDownList(bool value) { SetFlag(DIF_DROPDOWNLIST, value); }

protected:
    virtual long ItemProc(int Msg, long Param);
    virtual void Init();

private:
    TFarList *FList;
};
//---------------------------------------------------------------------------
class TFarLister : public TFarDialogItem
{
    typedef TFarLister self;
public:
    explicit TFarLister(TFarDialog *ADialog);
    virtual ~TFarLister();

    TStrings *GetItems();
    void SetItems(TStrings *value);
    int GetTopIndex() { return FTopIndex; }
    void SetTopIndex(int value);
    bool GetScrollBar();

protected:
    virtual long ItemProc(int Msg, long Param);
    virtual void DoFocus();

private:
    TStringList *FItems;
    int FTopIndex;

    void ItemsChange(TObject *Sender);
};
//---------------------------------------------------------------------------
std::wstring StripHotKey(std::wstring Text);
TRect Rect(int Left, int Top, int Right, int Bottom);
//---------------------------------------------------------------------------
