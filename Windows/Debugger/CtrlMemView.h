// NOTE: Apologies for the quality of this code, this is really from pre-opensource Dolphin - that is, 2003.

#pragma once

//////////////////////////////////////////////////////////////////////////
//CtrlDisAsmView
// CtrlDisAsmView.cpp
//////////////////////////////////////////////////////////////////////////
//This Win32 control is made to be flexible and usable with
//every kind of CPU architecture that has fixed width instruction words.
//Just supply it an instance of a class derived from Debugger, with all methods
//overridden for full functionality.
//
//To add to a dialog box, just draw a User Control in the dialog editor,
//and set classname to "CtrlDisAsmView". you also need to call CtrlDisAsmView::init()
//before opening this dialog, to register the window class.
//
//To get a class instance to be able to access it, just use getFrom(HWND wnd).

#include "../../Core/Debugger/DebugInterface.h"

// For registering clicks on or underlining specific nibbles in the hex view
#define HIGH_NIBBLE    0
#define LOW_NIBBLE     1
#define SPACE          2

// Spacing and Layout
// These numbers will be multiplied by charWidth
#define ADDRESS_PAD    1 // Distance beween left of frame and start of addresses
#define HEX_PAD        9 // Distance between end of addresses and start of hex view
#define ASCII_PAD      1 // Distance between end of hex view and start of ASCII view
#define HEX_ELEM_SIZE  3 // A high nibble, a low nibble and a space

class CtrlMemView
{
	HWND wnd;
	HFONT font;
	HFONT underlineFont;
	RECT rect;

	unsigned int curAddress;
	unsigned int windowStart;
	int rowHeight;
	int rowSize;

	int addressStart;
	int charWidth;
	int hexStart;
	int asciiStart;
	bool asciiSelected;
	int selectedNibble;

	int visibleRows;
	
	std::string searchQuery;
	int matchAddress;
	bool searching;

	bool hasFocus;
	static wchar_t szClassName[];
	DebugInterface *debugger;
	void updateStatusBarText();
	void search(bool continueSearch);
public:
	CtrlMemView(HWND _wnd);
	~CtrlMemView();
	static void init();
	static void deinit();
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static CtrlMemView * getFrom(HWND wnd);

	void setDebugger(DebugInterface *deb)
	{
		debugger=deb;
	}
	DebugInterface *getDebugger()
	{
		return debugger;
	}

	void onPaint(WPARAM wParam, LPARAM lParam);
	void onVScroll(WPARAM wParam, LPARAM lParam);
	void onKeyDown(WPARAM wParam, LPARAM lParam);
	void onChar(WPARAM wParam, LPARAM lParam);
	void onMouseDown(WPARAM wParam, LPARAM lParam, int button);
	void onMouseUp(WPARAM wParam, LPARAM lParam, int button);
	void onMouseMove(WPARAM wParam, LPARAM lParam, int button);
	void redraw();

	void gotoPoint(int x, int y);
	void gotoAddr(unsigned int addr);
	void scrollWindow(int lines);
	void scrollCursor(int bytes);
};