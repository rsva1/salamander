﻿// SPDX-FileCopyrightText: 2023 Open Salamander Authors
// SPDX-License-Identifier: GPL-2.0-or-later
// CommentsTranslationProject: TRANSLATED

#pragma once

//****************************************************************************
//
// CProgressBar
//
// The class is always allocated (CObjectOrigin origin = ooAllocated)

class CProgressBar : public CWindow
{
public:
    // hDlg is the parent window (dialog or window)
    // ctrlID is the child window ID
    CProgressBar(HWND hDlg, int ctrlID);
    ~CProgressBar();

    // SetProgress can be called from any thread and internally sends WM_USER_SETPROGRESS
    // however, thread progress bars must be running
    void SetProgress(DWORD progress, const char* text = NULL);
    void SetProgress2(const CQuadWord& progressCurrent, const CQuadWord& progressTotal,
                      const char* text = NULL);

    void SetSelfMoveTime(DWORD time);
    void SetSelfMoveSpeed(DWORD moveTime);
    void Stop();

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Paint(HDC hDC);

    void MoveBar();

protected:
    int Width, Height;
    DWORD Progress;
    CBitmap* Bitmap;     // bitmap used by memDC -> drawing cache
    int BarX;            // X coordinate of the rectangle for indeterminate progress (Progress == -1)
    BOOL MoveBarRight;   // is the rectangle moving to the right?
    DWORD SelfMoveTime;  // 0: after SetProgress(-1) the bar moves only one step (default value)
                         // >0: time in ms the bar continues to move after SetProgress(-1)
    DWORD SelfMoveTicks; // stored GetTickCount() value from the last SetSelfMoveTime() call
    DWORD SelfMoveSpeed; // how fast the bar moves: value in ms determining when the rectangle shifts
                         // minimum is 10 ms, default is 50 ms -- 20 steps per second
                         // beware of low values; the animation alone can significantly load the CPU
    BOOL TimerIsRunning; // is the timer running?
    char* Text;          // displayed instead of the numeric value when not NULL
    HFONT HFont;         // font for the progress bar
};

//****************************************************************************
//
// CStaticText
//
// The class is always allocated (CObjectOrigin origin = ooAllocated)

class CStaticText : public CWindow
{
public:
    // hDlg is the parent window (dialog or window)
    // ctrlID is the child window ID
    // flags is a combination of STF_* values (shared\spl_gui.h)
    CStaticText(HWND hDlg, int ctrlID, DWORD flags);
    ~CStaticText();

    // sets Text; returns TRUE on success and FALSE on out-of-memory
    BOOL SetText(const char* text);

    // note that the returned Text may be NULL
    const char* GetText() { return Text; }

    // sets Text; if it begins or ends with a space, it is wrapped in double quotes
    // returns TRUE on success and FALSE when memory allocation fails
    BOOL SetTextToDblQuotesIfNeeded(const char* text);

    // on some filesystems a different path separator may be required
    // the separator must not be '\0'
    void SetPathSeparator(char separator);

    // assigns the text that will be shown as a tooltip
    BOOL SetToolTipText(const char* text);

    // assigns the window and id that will receive WM_USER_TTGETTEXT when the tooltip is shown
    void SetToolTip(HWND hNotifyWindow, DWORD id);

    // if set to TRUE the tooltip can be triggered by clicking the text or
    // pressing Up/Down/Space when the control has focus; the tooltip then
    // appears right under the text and stays visible; the default is FALSE
    void EnableHintToolTip(BOOL enable);

    //    void UpdateControl();

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void PrepareForPaint();

    BOOL TextHitTest(POINT* screenCursorPos);
    int GetTextXOffset(); // returns the X offset based on Alignment, Width, and TextWidth
    void DrawFocus(HDC hDC);

    BOOL ToolTipAssigned();

    BOOL ShowHint();

    DWORD Flags;         // flags specifying the control behavior
    char* Text;          // allocated text
    int TextLen;         // string length
    char* Text2;         // allocated text containing an ellipsis; used only with STF_END_ELLIPSIS or STF_PATH_ELLIPSIS
    int Text2Len;        // length of Text2
    int* AlpDX;          // array of substring lengths; used only with STF_END_ELLIPSIS or STF_PATH_ELLIPSIS
    int TextWidth;       // text width in points
    int TextHeight;      // text height in points
    int Allocated;       // size of the allocated 'Text' and 'AlpDX' buffers
    int Width, Height;   // dimensions of the static control
    CBitmap* Bitmap;     // drawing cache; used only with STF_CACHED_PAINT
    HFONT HFont;         // font handle used for drawing text
    BOOL DestroyFont;    // TRUE if HFont was allocated, otherwise FALSE
    BOOL ClipDraw;       // we need to clip the drawing so that we don't paint outside
    BOOL Text2Draw;      // we will draw from the buffer containing an ellipsis
    int Alignment;       // 0=left, 1=center, 2=right
    char PathSeparator;  // separator of path segments; default '\\'
    BOOL MouseIsTracked; // mouse leave tracking has been installed
    // tooltip support
    char* ToolTipText; // string that will be shown as our tooltip
    HWND HToolTipNW;   // notification window
    DWORD ToolTipID;   // ID under which the tooltip asks for text
    BOOL HintMode;     // should the tooltip be displayed as a hint?
    WORD UIState;      // displaying accelerators
};

//****************************************************************************
//
// CHyperLink
//

class CHyperLink : public CStaticText
{
public:
    // hDlg is the parent window (dialog or window)
    // ctrlID is the child window ID
    // flags is a combination of STF_* values (shared\spl_gui.h)
    CHyperLink(HWND hDlg, int ctrlID, DWORD flags = STF_UNDERLINE | STF_HYPERLINK_COLOR);

    void SetActionOpen(const char* file);
    void SetActionPostCommand(WORD command);
    BOOL SetActionShowHint(const char* text);

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnContextMenu(int x, int y);
    BOOL ExecuteIt();

protected:
    char File[MAX_PATH]; // when non-zero it is passed to ShellExecute
    WORD Command;        // when non-zero this command is executed during the action
    HWND HDialog;        // parent dialog
};

//****************************************************************************
//
// CColorRectangle
//
// draws the entire object area with the color Color
// combine with WS_EX_CLIENTEDGE
//

class CColorRectangle : public CWindow
{
protected:
    COLORREF Color;

public:
    CColorRectangle(HWND hDlg, int ctrlID, CObjectOrigin origin = ooAllocated);

    void SetColor(COLORREF color);

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void PaintFace(HDC hdc);
};

//****************************************************************************
//
// CColorGraph
//

class CColorGraph : public CWindow
{
protected:
    HBRUSH Color1Light;
    HBRUSH Color1Dark;
    HBRUSH Color2Light;
    HBRUSH Color2Dark;

    RECT ClientRect;
    double UsedProc;

public:
    CColorGraph(HWND hDlg, int ctrlID, CObjectOrigin origin = ooAllocated);
    ~CColorGraph();

    void SetColor(COLORREF color1Light, COLORREF color1Dark,
                  COLORREF color2Light, COLORREF color2Dark);

    void SetUsed(double used); // used = <0, 1>

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void PaintFace(HDC hdc);
};

//****************************************************************************
//
// CBitmapButton
//

class CButton : public CWindow
{
protected:
    DWORD Flags;
    BOOL DropDownPressed;
    BOOL Checked;
    BOOL ButtonPressed;
    BOOL Pressed;
    BOOL DefPushButton;
    BOOL Captured;
    BOOL Space;
    RECT ClientRect;
    // tooltip support
    BOOL MouseIsTracked;  // mouse leave tracking has been installed
    char* ToolTipText;    // string that will be shown as our tooltip
    HWND HToolTipNW;      // notification window
    DWORD ToolTipID;      // ID under which the tooltip asks for text
    DWORD DropDownUpTime; // time in [ms] when the drop-down was released, used to prevent immediate re-press
    // XP Theme support
    BOOL Hot;
    WORD UIState; // displaying accelerators

public:
    CButton(HWND hDlg, int ctrlID, DWORD flags, CObjectOrigin origin = ooAllocated);
    ~CButton();

    // assigns the text that will be shown as a tooltip
    BOOL SetToolTipText(const char* text);

    // assigns the window and id that will receive WM_USER_TTGETTEXT when the tooltip is shown
    void SetToolTip(HWND hNotifyWindow, DWORD id);

    DWORD GetFlags();
    void SetFlags(DWORD flags, BOOL updateWindow);

protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void PaintFace(HDC hdc, const RECT* rect, BOOL enabled);

    int HitTest(LPARAM lParam); // returns 0: nowhere; 1: button; 2: drop-down
    void PaintFrame(HDC hDC, const RECT* r, BOOL down);
    void PaintDrop(HDC hDC, const RECT* r, BOOL enabled);
    int GetDropPartWidth();

    void RePaint();
    void NotifyParent(WORD notify);

    BOOL ToolTipAssigned();
};

//****************************************************************************
//
// CColorArrowButton
//
// a background with text with an arrow behind it that opens a menu
//

class CColorArrowButton : public CButton
{
protected:
    COLORREF TextColor;
    COLORREF BkgndColor;
    BOOL ShowArrow;

public:
    CColorArrowButton(HWND hDlg, int ctrlID, BOOL showArrow, CObjectOrigin origin = ooAllocated);

    void SetColor(COLORREF textColor, COLORREF bkgndColor);
    //    void     SetColor(COLORREF color);

    void SetTextColor(COLORREF textColor);
    void SetBkgndColor(COLORREF bkgndColor);

    COLORREF GetTextColor() { return TextColor; }
    COLORREF GetBkgndColor() { return BkgndColor; }

protected:
    virtual void PaintFace(HDC hdc, const RECT* rect, BOOL enabled);
};

//****************************************************************************
//
// CToolbarHeader
//

//#define TOOLBARHDR_USE_SVG

class CToolBar;

class CToolbarHeader : public CWindow
{
protected:
    CToolBar* ToolBar;
#ifdef TOOLBARHDR_USE_SVG
    HIMAGELIST HEnabledImageList;
    HIMAGELIST HDisabledImageList;
#else
    HIMAGELIST HHotImageList;
    HIMAGELIST HGrayImageList;
#endif
    DWORD ButtonMask;   // used buttons
    HWND HNotifyWindow; // where commands are sent
    WORD UIState;       // displaying accelerators

public:
    CToolbarHeader(HWND hDlg, int ctrlID, HWND hAlignWindow, DWORD buttonMask);

    void EnableToolbar(DWORD enableMask);
    void CheckToolbar(DWORD checkMask);
    void SetNotifyWindow(HWND hWnd) { HNotifyWindow = hWnd; }

protected:
#ifdef TOOLBARHDR_USE_SVG
    void CreateImageLists(HIMAGELIST* enabled, HIMAGELIST* disabled);
#endif

    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnPaint(HDC hDC, BOOL hideAccel, BOOL prefixOnly);
};

//****************************************************************************
//
// CAnimate
//

/*
class CAnimate: public CWindow
{
  protected:
    HBITMAP          HBitmap;             // bitmap containing the animation frames
    int              FramesCount;         // number of frames in the bitmap
    int              FirstLoopFrame;      // frame to jump to when looping
    SIZE             FrameSize;           // frame size in pixels
    CRITICAL_SECTION GDICriticalSection;  // critical section for access to GDI resources
    CRITICAL_SECTION DataCriticalSection; // critical section for access to data
    HANDLE           HThread;
    HANDLE           HRunEvent;           // when signaled the animation thread runs
    HANDLE           HTerminateEvent;     // when signaled the thread terminates
    COLORREF         BkColor;

    // control variables used when HRunEvent is signed
    BOOL             SleepThread;         // the thread should sleep; HRunEvent will be reset

    int              CurrentFrame;        // zero-based index of the currently displayed frame
    int              NestedCount;
    BOOL             MouseIsTracked;      // we have installed mouse leave tracking?

  public:
    // 'hBitmap'          bitmap used for drawing animation frames;
    //                    frames must be stacked vertically with a constant height
    // 'framesCount'      total number of frames in the bitmap
    // 'firstLoopFrame'   zero-based frame index to return to when cycling
    CAnimate(HBITMAP hBitmap, int framesCount, int firstLoopFrame, COLORREF bkColor, CObjectOrigin origin = ooAllocated);
    BOOL IsGood();                // did the constructor succeed?

    void Start();                 // start the animation if not already running
    void Stop();                  // stop the animation and show the first frame
    void GetFrameSize(SIZE *sz);  // returns the frame size in pixels

  protected:
    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Paint(HDC hdc = NULL);   // draw the current frame; grab the window DC if hdc is NULL
    void FirstFrame();            // set Frame to the initial cell
    void NextFrame();             // advance to the next frame, skip the intro sequence

    // thread bodies
    static unsigned ThreadF(void *param);
    static unsigned AuxThreadEH(void *param);
    static DWORD WINAPI AuxThreadF(void *param);

    // ThreadF is a friend so it can access our data
    friend static unsigned ThreadF(void *param);
};
*/

//
//  ****************************************************************************
// ChangeToArrowButton
//

BOOL ChangeToArrowButton(HWND hParent, int ctrlID);

//
//  ****************************************************************************
// ChangeToIconButton
//

BOOL ChangeToIconButton(HWND hParent, int ctrlID, int iconID);

//
//  ****************************************************************************
// VerticalAlignChildToChild
//
// aligns the "browse" button with the edit line / combobox (in the resource workshop it is hard to place the button behind a combobox)
// adjusts the size and position of the child window "alignID" so it is at the same height (and equally tall) as the child "toID"
void VerticalAlignChildToChild(HWND hParent, int alignID, int toID);

//
//  ****************************************************************************
// CondenseStaticTexts
//
// moves static texts so they directly follow each other; the spacing between them
// equals the width of one space in the dialog font; "staticsArr" is an array of
// static IDs terminated by zero
void CondenseStaticTexts(HWND hWindow, int* staticsArr);

//
//  ****************************************************************************
// ArrangeHorizontalLines
//
// finds horizontal lines in a dialog and extends them from the right to the
// text they relate to; it also locates check boxes and radio buttons acting as
// group-box labels and shortens them according to their text and the current
// dialog font, eliminating extra gaps caused by varying screen DPI
void ArrangeHorizontalLines(HWND hWindow);

//
//  ****************************************************************************
// GetWindowFontHeight
//
// retrieves the font currently used by hWindow and returns its height
int GetWindowFontHeight(HWND hWindow);

//
//  ****************************************************************************
// GetWindowFontHeight
//
// creates an image list containing two check box states (unchecked and checked)
// and returns the list handle; 'itemSize' specifies the width and height of each
// item in points
HIMAGELIST CreateCheckboxImagelist(int itemSize);

//
//  ****************************************************************************
// SalLoadIcon
//
// Loads the icon specified by 'hInst' and 'iconName', and returns its handle,
// or NULL in case of an error. 'iconSize' specifies the desired icon size.
// The function is High DPI–ready and returns the handle.
// 'itemSize' is the width and height of a single item in points.
//
// Note: the old LoadIcon() API cannot load larger icons, therefore we introduce this
// function that loads icons using the new LoadIconWithScaleDown()
HICON SalLoadIcon(HINSTANCE hInst, LPCTSTR iconName, CIconSizeEnum iconSize);
