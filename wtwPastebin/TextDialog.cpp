/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "stdinc.hpp"
#include "TextDialog.h"
#include "wtwPastebin.hpp"

#include "../utils/windows.hpp"
#include "../utils/WinApiFile.hpp"
#include "../utils/text.hpp"

#include <boost/format.hpp>

CEditor::CEditor()
{
	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	wcscpy(ncm.lfMessageFont.lfFaceName, L"Courier New");
	hFont = CreateFontIndirectW(&ncm.lfMessageFont);
}

CEditor::~CEditor()
{
	DeleteObject(hFont);
}

LRESULT TextDialog::onInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	hFont = utils::windows::getSystemFont();
	utils::windows::SetChildFont(m_hWnd, hFont);

	try
	{
		nick = boost::str(boost::wformat(L"wtwPastebin - wysy�anie do %1% (%2%)") % nick % id);
	}
	catch(...)
	{
		nick = L"wtwPastebin";
	}

	SetWindowText(nick.c_str());

	CComboBox c;
	c.Attach(GetDlgItem(IDC_FORMAT));
	addFormat(c, L"Czysty tekst", L"");
	addFormat(c, L"4CS", L"4cs");
	addFormat(c, L"6502 ACME Cross Assembler", L"6502acme");
	addFormat(c, L"6502 Kick Assembler", L"6502kickass");
	addFormat(c, L"6502 TASM/64TASS", L"6502tasm");
	addFormat(c, L"ABAP", L"abap");
	addFormat(c, L"ActionScript", L"actionscript");
	addFormat(c, L"ActionScript 3", L"actionscript3");
	addFormat(c, L"Ada", L"ada");
	addFormat(c, L"ALGOL 68", L"algol68");
	addFormat(c, L"Apache Log", L"apache");
	addFormat(c, L"AppleScript", L"applescript");
	addFormat(c, L"APT Sources", L"apt_sources");
	addFormat(c, L"ASM (NASM)", L"asm");
	addFormat(c, L"ASP", L"asp");
	addFormat(c, L"autoconf", L"autoconf");
	addFormat(c, L"Autohotkey", L"autohotkey");
	addFormat(c, L"AutoIt", L"autoit");
	addFormat(c, L"Avisynth", L"avisynth");
	addFormat(c, L"Awk", L"awk");
	addFormat(c, L"Bash", L"bash");
	addFormat(c, L"Basic4GL", L"basic4gl");
	addFormat(c, L"BibTeX", L"bibtex");
	addFormat(c, L"Blitz Basic", L"blitzbasic");
	addFormat(c, L"BNF", L"bnf");
	addFormat(c, L"BOO", L"boo");
	addFormat(c, L"BrainFuck", L"bf");
	addFormat(c, L"C", L"c");
	addFormat(c, L"C for Macs", L"c_mac");
	addFormat(c, L"C Intermediate Language", L"cil");
	addFormat(c, L"C#", L"csharp");
	addFormat(c, L"C++", L"cpp");
	addFormat(c, L"C++ (with QT extensions)", L"cpp-qt");
	addFormat(c, L"CAD DCL", L"caddcl");
	addFormat(c, L"CAD Lisp", L"cadlisp");
	addFormat(c, L"CFDG", L"cfdg");
	addFormat(c, L"ChaiScript", L"chaiscript");
	addFormat(c, L"Clojure", L"clojure");
	addFormat(c, L"Clone C", L"klonec");
	addFormat(c, L"Clone C++", L"klonecpp");
	addFormat(c, L"CMake", L"cmake");
	addFormat(c, L"COBOL", L"cobol");
	addFormat(c, L"ColdFusion", L"cfm");
	addFormat(c, L"CSS", L"css");
	addFormat(c, L"Cuesheet", L"cuesheet");
	addFormat(c, L"D", L"d");
	addFormat(c, L"DCS", L"dcs");
	addFormat(c, L"Delphi", L"delphi");
	addFormat(c, L"Delphi Prism (Oxygene)", L"oxygene");
	addFormat(c, L"Diff", L"diff");
	addFormat(c, L"DIV", L"div");
	addFormat(c, L"DOS", L"dos");
	addFormat(c, L"DOT", L"dot");
	addFormat(c, L"E", L"e");
	addFormat(c, L"ECMAScript", L"ecmascript");
	addFormat(c, L"Eiffel", L"eiffel");
	addFormat(c, L"Email", L"email");
	addFormat(c, L"Erlang", L"erlang");
	addFormat(c, L"F#", L"fsharp");
	addFormat(c, L"FO Language", L"fo");
	addFormat(c, L"Formula One", L"f1");
	addFormat(c, L"Fortran", L"fortran");
	addFormat(c, L"FreeBasic", L"freebasic");
	addFormat(c, L"GAMBAS", L"gambas");
	addFormat(c, L"Game Maker", L"gml");
	addFormat(c, L"GDB", L"gdb");
	addFormat(c, L"Genero", L"genero");
	addFormat(c, L"Genie", L"genie");
	addFormat(c, L"GetText", L"gettext");
	addFormat(c, L"Go", L"go");
	addFormat(c, L"Groovy", L"groovy");
	addFormat(c, L"GwBasic", L"gwbasic");
	addFormat(c, L"Haskell", L"haskell");
	addFormat(c, L"HicEst", L"hicest");
	addFormat(c, L"HQ9 Plus", L"hq9plus");
	addFormat(c, L"HTML", L"html4strict");
	addFormat(c, L"Icon", L"icon");
	addFormat(c, L"IDL", L"idl");
	addFormat(c, L"INI file", L"ini");
	addFormat(c, L"Inno Script", L"inno");
	addFormat(c, L"INTERCAL", L"intercal");
	addFormat(c, L"IO", L"io");
	addFormat(c, L"J", L"j");
	addFormat(c, L"Java", L"java");
	addFormat(c, L"Java 5", L"java5");
	addFormat(c, L"JavaScript", L"javascript");
	addFormat(c, L"jQuery", L"jquery");
	addFormat(c, L"KiXtart", L"kixtart");
	addFormat(c, L"Latex", L"latex");
	addFormat(c, L"Liberty BASIC", L"lb");
	addFormat(c, L"Linden Scripting", L"lsl2");
	addFormat(c, L"Lisp", L"lisp");
	addFormat(c, L"Loco Basic", L"locobasic");
	addFormat(c, L"Logtalk", L"logtalk");
	addFormat(c, L"LOL Code", L"lolcode");
	addFormat(c, L"Lotus Formulas", L"lotusformulas");
	addFormat(c, L"Lotus Script", L"lotusscript");
	addFormat(c, L"LScript", L"lscript");
	addFormat(c, L"Lua", L"lua");
	addFormat(c, L"M68000 Assembler", L"m68k");
	addFormat(c, L"MagikSF", L"magiksf");
	addFormat(c, L"Make", L"make");
	addFormat(c, L"MapBasic", L"mapbasic");
	addFormat(c, L"MatLab", L"matlab");
	addFormat(c, L"mIRC", L"mirc");
	addFormat(c, L"MIX Assembler", L"mmix");
	addFormat(c, L"Modula 2", L"modula2");
	addFormat(c, L"Modula 3", L"modula3");
	addFormat(c, L"Motorola 68000 HiSoft Dev", L"68000devpac");
	addFormat(c, L"MPASM", L"mpasm");
	addFormat(c, L"MXML", L"mxml");
	addFormat(c, L"MySQL", L"mysql");
	addFormat(c, L"newLISP", L"newlisp");
	addFormat(c, L"None", L"text");
	addFormat(c, L"NullSoft Installer", L"nsis");
	addFormat(c, L"Oberon 2", L"oberon2");
	addFormat(c, L"Objeck Programming Langua", L"objeck");
	addFormat(c, L"Objective C", L"objc");
	addFormat(c, L"OCalm Brief", L"ocaml-brief");
	addFormat(c, L"OCaml", L"ocaml");
	addFormat(c, L"OpenBSD PACKET FILTER", L"pf");
	addFormat(c, L"OpenGL Shading", L"glsl");
	addFormat(c, L"Openoffice BASIC", L"oobas");
	addFormat(c, L"Oracle 11", L"oracle11");
	addFormat(c, L"Oracle 8", L"oracle8");
	addFormat(c, L"Oz", L"oz");
	addFormat(c, L"Pascal", L"pascal");
	addFormat(c, L"PAWN", L"pawn");
	addFormat(c, L"PCRE", L"pcre");
	addFormat(c, L"Per", L"per");
	addFormat(c, L"Perl", L"perl");
	addFormat(c, L"Perl 6", L"perl6");
	addFormat(c, L"PHP", L"php");
	addFormat(c, L"PHP Brief", L"php-brief");
	addFormat(c, L"Pic 16", L"pic16");
	addFormat(c, L"Pike", L"pike");
	addFormat(c, L"Pixel Bender", L"pixelbender");
	addFormat(c, L"PL/SQL", L"plsql");
	addFormat(c, L"PostgreSQL", L"postgresql");
	addFormat(c, L"POV-Ray", L"povray");
	addFormat(c, L"Power Shell", L"powershell");
	addFormat(c, L"PowerBuilder", L"powerbuilder");
	addFormat(c, L"Progress", L"progress");
	addFormat(c, L"Prolog", L"prolog");
	addFormat(c, L"Properties", L"properties");
	addFormat(c, L"ProvideX", L"providex");
	addFormat(c, L"PureBasic", L"purebasic");
	addFormat(c, L"Python", L"python");
	addFormat(c, L"q/kdb+", L"q");
	addFormat(c, L"QBasic", L"qbasic");
	addFormat(c, L"R", L"rsplus");
	addFormat(c, L"Rails", L"rails");
	addFormat(c, L"REBOL", L"rebol");
	addFormat(c, L"REG", L"reg");
	addFormat(c, L"Robots", L"robots");
	addFormat(c, L"RPM Spec", L"rpmspec");
	addFormat(c, L"Ruby", L"ruby");
	addFormat(c, L"Ruby Gnuplot", L"gnuplot");
	addFormat(c, L"SAS", L"sas");
	addFormat(c, L"Scala", L"scala");
	addFormat(c, L"Scheme", L"scheme");
	addFormat(c, L"Scilab", L"scilab");
	addFormat(c, L"SdlBasic", L"sdlbasic");
	addFormat(c, L"Smalltalk", L"smalltalk");
	addFormat(c, L"Smarty", L"smarty");
	addFormat(c, L"SQL", L"sql");
	addFormat(c, L"SystemVerilog", L"systemverilog");
	addFormat(c, L"T-SQL", L"tsql");
	addFormat(c, L"TCL", L"tcl");
	addFormat(c, L"Tera Term", L"teraterm");
	addFormat(c, L"thinBasic", L"thinbasic");
	addFormat(c, L"TypoScript", L"typoscript");
	addFormat(c, L"Unicon", L"unicon");
	addFormat(c, L"Vala", L"vala");
	addFormat(c, L"VB.NET", L"vbnet");
	addFormat(c, L"VeriLog", L"verilog");
	addFormat(c, L"VHDL", L"vhdl");
	addFormat(c, L"VIM", L"vim");
	addFormat(c, L"Visual Pro Log", L"visualprolog");
	addFormat(c, L"VisualBasic", L"vb");
	addFormat(c, L"VisualFoxPro", L"visualfoxpro");
	addFormat(c, L"WhiteSpace", L"whitespace");
	addFormat(c, L"WHOIS", L"whois");
	addFormat(c, L"Win Batch", L"winbatch");
	addFormat(c, L"XBasic", L"xbasic");
	addFormat(c, L"XML", L"xml");
	addFormat(c, L"Xorg Config", L"xorg_conf");
	addFormat(c, L"XPP", L"xpp");
	addFormat(c, L"Z80 Assembler", L"z80");
	addFormat(c, L"ZXBasic", L"zxbasic");
	c.SetCurSel(0);
	c.Detach();

	c.Attach(GetDlgItem(IDC_EXPIRE));
	addFormat(c, L"Nigdy", L"N");
	addFormat(c, L"10 minut", L"10M");
	addFormat(c, L"1 godzina", L"1H");
	addFormat(c, L"1 dzie�", L"1D");
	addFormat(c, L"1 miesi�c", L"1M");
	c.SetCurSel(3);
	c.Detach();

	ctrlEdit.SubclassWindow(GetDlgItem(IDC_TEXT));
	ctrlEdit.SetFont(ctrlEdit.hFont);

	CRect rc;
	GetWindowRect(&rc);
	minWindowSize.x = rc.Width();
	minWindowSize.y = rc.Height();

	return 0;
}

LRESULT TextDialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ctrlEdit.UnsubclassWindow();

	int i;
	CComboBox c;
	c.Attach(GetDlgItem(IDC_FORMAT));
	
	for(i = 0; i < c.GetCount(); ++i)
	{
		delete (std::string*)c.GetItemDataPtr(i);
	}

	c.Clear();
	c.Detach();

	c.Attach(GetDlgItem(IDC_EXPIRE));

	for(i = 0; i < c.GetCount(); ++i)
	{
		delete (std::string*)c.GetItemDataPtr(i);
	}

	c.Clear();
	c.Detach();

	DeleteObject(hFont);
	return 0;
}

LRESULT TextDialog::onSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rc;
	CRect rcEdit;
	int height = 0;

	GetWindowRect(&rc);
	ctrlEdit.GetWindowRect(&rcEdit);
	height = rc.Height() - (rcEdit.top - rc.top);

	ctrlEdit.SetWindowPos(0, 0, 0, rc.Width() - 38, height - 20, SWP_NOMOVE | SWP_NOZORDER);

	return 0;
}

LRESULT TextDialog::onMinMaxSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	MINMAXINFO* info = (MINMAXINFO*)lParam;

	if(info != 0)
	{
		info->ptMinTrackSize = minWindowSize;
	}

	return 0;
}

LRESULT TextDialog::onLoadFile(WORD /* wNotifyCode */, WORD /*wID*/, HWND /* hWndCtl */, BOOL& /* bHandled */)
{
	std::wstring filePath;

	if(WTL::RunTimeHelper::IsVista())
	{
		IFileDialog* pfd;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		
		if(SUCCEEDED(hr))
		{
			DWORD opt;
			hr = pfd->GetOptions(&opt);
			
			if(SUCCEEDED(hr))
			{
				pfd->SetOptions(opt | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
			}
			
			COMDLG_FILTERSPEC files;
			files.pszName = L"Wszystkie pliki";
			files.pszSpec = L"*.*";

			pfd->SetFileTypes(1, &files);

			hr = pfd->Show(0);
			
			if(SUCCEEDED(hr))
			{
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);
				
				if(SUCCEEDED(hr))
				{
					LPOLESTR pwsz = NULL;

					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
					
					if(SUCCEEDED(hr))
					{
						filePath = pwsz;
						CoTaskMemFree(pwsz);
					}

					psiResult->Release();
				}
			}

			pfd->Release();
		}
	}
	else
	{
		CFileDialog fd(TRUE, 0, 0, 4 | 2, L"Wszystkie pliki\0*.*\0");
		
		if(fd.DoModal() == IDOK)
		{
			filePath = fd.m_szFileName;
		}
	}

	if(filePath.size())
	{
		wtwPastebin::pasteItem pi;
		WinApiFile f;
		
		if(f.open(filePath, WinApiFile::READ_MODE))
		{
			pi.data.resize(f.getFileSize());
			f.read(&pi.data[0], pi.data.size());

			CComboBox c;
			c.Attach(GetDlgItem(IDC_FORMAT));
			std::wstring* format = (std::wstring*)c.GetItemDataPtr(c.GetCurSel());
			c.Detach();
			c.Attach(GetDlgItem(IDC_EXPIRE));
			std::wstring* expire = (std::wstring*)c.GetItemDataPtr(c.GetCurSel());
			c.Detach();

			pi.format = *format;
			pi.expire = *expire;

			CButton btn;
			btn.Attach(GetDlgItem(IDC_PRIVATE));
			pi.priv = btn.GetCheck() ? true : false;
			btn.Detach();

			wtwPastebin::instance()->send(id.c_str(), netClass.c_str(), netId, pi);
			
			EndDialog(0);
		}
	}

	return 0;
}

LRESULT TextDialog::onClose(WORD /* wNotifyCode */, WORD wID, HWND /* hWndCtl */, BOOL& /* bHandled */) {
	if(wID == IDOK)
	{
		std::wstring buf;
		buf.resize(::GetWindowTextLength(GetDlgItem(IDC_TEXT)) + 1);
		buf.resize(::GetWindowText(GetDlgItem(IDC_TEXT), &buf[0], buf.size()));

		if(wcslen(buf.c_str()) > 0)
		{
			wtwPastebin::pasteItem pi;
			pi.data = utils::text::fromWide(buf);

			CComboBox c;
			c.Attach(GetDlgItem(IDC_FORMAT));
			std::wstring* format = (std::wstring*)c.GetItemDataPtr(c.GetCurSel());
			c.Detach();
			c.Attach(GetDlgItem(IDC_EXPIRE));
			std::wstring* expire = (std::wstring*)c.GetItemDataPtr(c.GetCurSel());
			c.Detach();

			pi.format = *format;
			pi.expire = *expire;

			CButton btn;
			btn.Attach(GetDlgItem(IDC_PRIVATE));
			pi.priv = btn.GetCheck() ? true : false;
			btn.Detach();

			wtwPastebin::instance()->send(id.c_str(), netClass.c_str(), netId, pi);
		}
	}

	EndDialog(wID);
	return 0;
}

void TextDialog::addFormat(CComboBox& c, const wchar_t* name, const wchar_t* format)
{
	std::wstring* f = new std::wstring(format);
	int i = c.AddString(name);
	c.SetItemDataPtr(i, (void*)f);
}
