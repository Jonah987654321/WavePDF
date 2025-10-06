#include <iostream>
#include <wx/wx.h>
#include "utility/PdfReader.h"

enum {
    ID_Hello = 1,
    ID_OPEN_FILE = 2
};

class MyFrame : public wxFrame {
    public:
        MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World") {
            wxMenu *menuFile = new wxMenu;
            menuFile->Append(ID_OPEN_FILE, "&Open\tCtrl-O");
            menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                             "Help string shown in status bar for this menu item");
            menuFile->AppendSeparator();
            menuFile->Append(wxID_EXIT);
            
            wxMenu *menuHelp = new wxMenu;
            menuHelp->Append(wxID_ABOUT);
            
            wxMenuBar *menuBar = new wxMenuBar;
            menuBar->Append(menuFile, "&File");
            menuBar->Append(menuHelp, "&Help");
            
            SetMenuBar( menuBar );
            
            CreateStatusBar();
            SetStatusText("Welcome to wxWidgets!");
            
            Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
            Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
            Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
            Bind(wxEVT_MENU, &MyFrame::OnOpenFile, this, ID_OPEN_FILE);
        }
 
    private:
        void OnHello(wxCommandEvent& event) {
            wxLogMessage("Hello world from wxWidgets!");
        };
        void OnExit(wxCommandEvent& event) {
            Close(true);
        }
        void OnAbout(wxCommandEvent& event) {
            wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
        }
        void OnOpenFile(wxCommandEvent& event) {
            wxLogDebug("Open File triggered");
            wxFileDialog openFileDialog(this, _("Select a PDF to open"), "", "", "PDF Files (*.pdf)|*.pdf", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

            if (openFileDialog.ShowModal() == wxID_CANCEL)
                return;     // user cancelled dialog

            PdfReader pdf(openFileDialog.GetPath());

            if (!pdf.process()) {
                wxMessageBox(_(pdf.getErrorMessage()),
                    _("Error"),
                    wxOK | wxICON_ERROR);
                return;
            }
        }
};

class MyApp : public wxApp {
    public:
        bool OnInit() override {
            wxLog::SetActiveTarget(new wxLogStderr());
            wxLogDebug("Debug output enabled!");

            MyFrame *frame = new MyFrame();
            frame->Show();
            return true;
        }
};

wxIMPLEMENT_APP(MyApp);
