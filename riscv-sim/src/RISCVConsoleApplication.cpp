#include "RISCVConsoleApplication.h"
#include "FileDataSink.h"
#include "FileDataSource.h"
#include "Path.h"
#include <cstdio>
#include <sstream>
#include <iomanip>

std::shared_ptr< CRISCVConsoleApplication > CRISCVConsoleApplication::DApplicationPointer;

CRISCVConsoleApplication::CRISCVConsoleApplication(const std::string &appname, const SPrivateConstructionType &key){
    DFileOpenFolder = CPath::CurrentPath().ToString();
    DApplication = CGUIFactory::ApplicationInstance(appname);
    
    DRISCVConsole = std::make_shared<CRISCVConsole>();

    DApplication->SetActivateCallback(this, ActivateCallback);
    
}

CRISCVConsoleApplication::~CRISCVConsoleApplication(){
    
}

void CRISCVConsoleApplication::ActivateCallback(TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    App->Activate();
}

bool CRISCVConsoleApplication::TimeoutCallback(TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    if(App->DDeleted){
        return false;   
    }
    
    return App->Timeout();
}

bool CRISCVConsoleApplication::MainWindowDeleteEventCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->MainWindowDeleteEvent(widget);
}

void CRISCVConsoleApplication::MainWindowDestroyCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    App->MainWindowDestroy(widget);
}

bool CRISCVConsoleApplication::MainWindowKeyPressEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->MainWindowKeyPressEvent(widget,event);
}

bool CRISCVConsoleApplication::MainWindowKeyReleaseEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->MainWindowKeyReleaseEvent(widget,event);
}

bool CRISCVConsoleApplication::MainWindowConfigureEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->MainWindowConfigureEvent(widget,event);
}

bool CRISCVConsoleApplication::DrawingAreaDrawCallback(std::shared_ptr<CGUIWidget> widget, std::shared_ptr<CGraphicResourceContext> rc, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->DrawingAreaDraw(widget, rc);
}

bool CRISCVConsoleApplication::FirmwareButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->FirmwareButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::CartridgeButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->CartridgeButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::EjectButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->EjectButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::ControllerButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->ControllerButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::CommandButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->CommandButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::ResetButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->ResetButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::PowerButtonToggledEventCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->PowerButtonToggledEvent(widget);
}

bool CRISCVConsoleApplication::SliderValueChangedEventCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->SliderValueChangedEvent(widget);
}

bool CRISCVConsoleApplication::DebugMemoryButtonClickEventCallback(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->DebugMemoryButtonClickEvent(widget,event);
}

bool CRISCVConsoleApplication::DebugMemoryStackButtonToggledEventCallback(std::shared_ptr<CGUIWidget> widget, TGUICalldata data){
    CRISCVConsoleApplication *App = static_cast<CRISCVConsoleApplication *>(data);
    return App->DebugMemoryStackButtonToggledEvent(widget);
}

void CRISCVConsoleApplication::Activate(){
    DMainWindow = DApplication->NewWindow();
    DMainWindow->SetDeleteEventCallback(this, MainWindowDeleteEventCallback);
    DMainWindow->SetDestroyEventCallback(this, MainWindowDestroyCallback);
   
    DMainWindow->SetKeyPressEventCallback(this, MainWindowKeyPressEventCallback);
    DMainWindow->SetKeyReleaseEventCallback(this, MainWindowKeyReleaseEventCallback);    

    CreateConsoleWidgets();
    
    if(DDebugMode){
        CreateDebugWidgets();
    }

    DMainWindow->Add(DConsoleDebugBox ? DConsoleDebugBox : DConsoleBox);
    DMainWindow->ShowAll();
    
    DApplication->SetTimer(10,this,TimeoutCallback);
}

bool CRISCVConsoleApplication::Timeout(){
    if(DRISCVConsole->VideoTimerTick(DDoubleBufferSurface)){
        DConsoleVideo->Invalidate();
    }

    return true;
}

bool CRISCVConsoleApplication::MainWindowDeleteEvent(std::shared_ptr<CGUIWidget> widget){
    DDeleted = true;
    return false;  
}

void CRISCVConsoleApplication::MainWindowDestroy(std::shared_ptr<CGUIWidget> widget){
    DMainWindow = nullptr;
}

bool CRISCVConsoleApplication::MainWindowKeyPressEvent(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event){
    auto ButtonSearch = DKeyControllerMapping.find(event.DValue.DValue);
    if(ButtonSearch != DKeyControllerMapping.end()){
        ButtonSearch->second->SetActive(true);
        UpdateConsoleButtonChange(ButtonSearch->second);
    }
    return true;
}

bool CRISCVConsoleApplication::MainWindowKeyReleaseEvent(std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event){
    auto ButtonSearch = DKeyControllerMapping.find(event.DValue.DValue);
    if(ButtonSearch != DKeyControllerMapping.end()){
        ButtonSearch->second->SetActive(false);
        UpdateConsoleButtonChange(ButtonSearch->second);
    }
    return true;
}

bool CRISCVConsoleApplication::MainWindowConfigureEvent(std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event){
    return true;
}

bool CRISCVConsoleApplication::DrawingAreaDraw(std::shared_ptr<CGUIWidget> widget, std::shared_ptr<CGraphicResourceContext> rc){
    rc->DrawSurface(DDoubleBufferSurface, 0, 0, -1, -1, 0, 0);
    return true;
}

bool CRISCVConsoleApplication::FirmwareButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    std::string Filename;
    {
        auto FileChooser = CGUIFactory::NewFileChooserDialog("Select Firmware",true,DMainWindow);
        FileChooser->SetCurrentFolder(DFileOpenFolder);
        if(FileChooser->Run()){
            Filename = FileChooser->GetFilename();
            DFileOpenFolder = FileChooser->GetCurrentFolder();
            auto InFile = std::make_shared<CFileDataSource>(Filename);
            if(DRISCVConsole->ProgramFirmware(InFile)){
                if(DDebugMode){
                    DDebugInstructions->SetBufferedLines(DRISCVConsole->InstructionStrings());
                    DFollowingInstruction = true;
                    RefreshDebugRegisters();
                }
            }
        }
    }
    

    return true;
}

bool CRISCVConsoleApplication::CartridgeButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    std::string Filename;
    {
        auto FileChooser = CGUIFactory::NewFileChooserDialog("Select Cartridge",true,DMainWindow);
        FileChooser->SetCurrentFolder(DFileOpenFolder);
        if(FileChooser->Run()){
            Filename = FileChooser->GetFilename();
            DFileOpenFolder = FileChooser->GetCurrentFolder();
            auto InFile = std::make_shared<CFileDataSource>(Filename);
            if(DRISCVConsole->InsertCartridge(InFile)){
                if(DDebugMode){
                    DDebugInstructions->SetBufferedLines(DRISCVConsole->InstructionStrings());
                    RefreshDebugRegisters();
                }
            }
        }
    }
    return true;
}

bool CRISCVConsoleApplication::EjectButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    DRISCVConsole->RemoveCartridge();
    return true;
}

bool CRISCVConsoleApplication::ControllerButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    auto ToggleButton = std::dynamic_pointer_cast<CGUIToggleButton>(widget);
    ToggleButton->SetActive(!ToggleButton->GetActive());
    UpdateConsoleButtonChange(ToggleButton);
    return true;
}

bool CRISCVConsoleApplication::CommandButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    DRISCVConsole->PressCommand();
    return true;
}

bool CRISCVConsoleApplication::ResetButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    DRISCVConsole->Reset();
    return true;
}

bool CRISCVConsoleApplication::PowerButtonToggledEvent(std::shared_ptr<CGUIWidget> widget){
    if(std::dynamic_pointer_cast<CGUIToggleButton>(widget)->GetActive()){
        DRISCVConsole->PowerOn();
    }
    else{
        DRISCVConsole->PowerOff();
        if(DDebugMode){
            RefreshDebugRegisters();
        }
    }
    
    return true;
}

bool CRISCVConsoleApplication::SliderValueChangedEvent(std::shared_ptr<CGUIWidget> widget){
    auto Slider = std::dynamic_pointer_cast<CGUIScrollBar>(widget);
    printf("Slider = %lf\n",Slider->GetValue());
    return true;
}

void CRISCVConsoleApplication::UpdateConsoleButtonChange(std::shared_ptr<CGUIToggleButton> button){
    auto DirectionSearch = DDirectionButtonMapping.find(button);
    if(DirectionSearch != DDirectionButtonMapping.end()){
        if(button->GetActive()){
            DRISCVConsole->PressDirection(DirectionSearch->second);
        }
        else{
            DRISCVConsole->ReleaseDirection(DirectionSearch->second);
        }
        return;
    }
    auto ButtonSearch = DButtonNumberButtonMapping.find(button);
    if(ButtonSearch != DButtonNumberButtonMapping.end()){
        if(button->GetActive()){
            DRISCVConsole->PressButton(ButtonSearch->second);
        }
        else{
            DRISCVConsole->ReleaseButton(ButtonSearch->second);
        }
    }
}

bool CRISCVConsoleApplication::DebugMemoryButtonClickEvent(std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event){
    auto MemoryButton = std::dynamic_pointer_cast<CGUIButton>(widget);
    auto Search = DDebugMemoryButtonMapping.find(MemoryButton);
    if(Search != DDebugMemoryButtonMapping.end()){
        DDebugMemory->SetBaseAddress(Search->second,true);
    }
    else{
        auto RegisterBase = DRISCVConsole->CPU()->Register(DDebugMemoryGlobalPointerRegisterIndex);
        RegisterBase = RegisterBase < 0x800 ? RegisterBase : RegisterBase - 0x800;
        DDebugMemory->SetBaseAddress(RegisterBase,true);
        
    }
    if(DDebugMemoryStackButton->GetActive()){
        DDebugMemoryStackButton->SetActive(false);
    }
    return true;
}

bool CRISCVConsoleApplication::DebugMemoryStackButtonToggledEvent(std::shared_ptr<CGUIWidget> widget){
    auto StackButton = std::dynamic_pointer_cast<CGUIToggleButton>(widget);

    if(StackButton->GetActive()){
        auto RegisterBase = DRISCVConsole->CPU()->Register(DDebugMemoryStackPointerRegisterIndex);
        DDebugMemory->SetBaseAddress(RegisterBase,false);
        DDebugMemory->Refresh();
    }
    return false;
}

std::shared_ptr< CRISCVConsoleApplication > CRISCVConsoleApplication::Instance(const std::string &appname){
    if(!DApplicationPointer){
        DApplicationPointer = std::make_shared< CRISCVConsoleApplication >(appname, SPrivateConstructionType{});   
    }
    return DApplicationPointer;
}

void CRISCVConsoleApplication::CreateConsoleWidgets(){
    DConsoleBox = CGUIFactory::NewBox(CGUIBox::EOrientation::Vertical,5);
    DConsoleVideo = CGUIFactory::NewDrawingArea();
    CreateControllerWidgets();
    CreateSystemControlWidgets();

    DControlsBox = CGUIFactory::NewBox(CGUIBox::EOrientation::Horizontal,5);
    DControlsBox->PackStart(DControllerGrid,false,false,4);
    DControlsBox->PackStart(DSystemControlGrid,false,false,4);

    DConsoleBox->PackStart(DConsoleVideo,false,false,4);
    DConsoleBox->PackStart(DControlsBox,false,false,4);
    
    DDoubleBufferSurface = CGraphicFactory::CreateSurface(DRISCVConsole->ScreenWidth(), DRISCVConsole->ScreenHeight(), ESurfaceFormat::ARGB32);
    DWorkingBufferSurface = CGraphicFactory::CreateSurface(DRISCVConsole->ScreenWidth(), DRISCVConsole->ScreenHeight(), ESurfaceFormat::ARGB32);
    DConsoleVideo->SetSizeRequest(DRISCVConsole->ScreenWidth(), DRISCVConsole->ScreenHeight());
    DConsoleVideo->SetDrawEventCallback(this, DrawingAreaDrawCallback);
    auto TempRC = DDoubleBufferSurface->CreateResourceContext();
    TempRC->SetSourceRGB(0x0);
    TempRC->SetLineWidth(1);
    TempRC->Rectangle(0,0,DRISCVConsole->ScreenWidth(), DRISCVConsole->ScreenHeight());
    TempRC->StrokePreserve();
    TempRC->Fill();    
    DConsoleVideo->Invalidate();
}

void CRISCVConsoleApplication::CreateControllerWidgets(){
    DUpButton = CGUIFactory::NewToggleButton();
    DDownButton = CGUIFactory::NewToggleButton();
    DLeftButton = CGUIFactory::NewToggleButton();
    DRightButton = CGUIFactory::NewToggleButton();
    DButton1 = CGUIFactory::NewToggleButton();
    DButton2 = CGUIFactory::NewToggleButton();
    DButton3 = CGUIFactory::NewToggleButton();
    DButton4 = CGUIFactory::NewToggleButton();
    DCommandButton = CGUIFactory::NewButton();
    DControllerGrid = CGUIFactory::NewGrid();
    DControllerGrid->Attach(DUpButton,1,0,1,1);
    DControllerGrid->Attach(DLeftButton,0,1,1,1);
    DControllerGrid->Attach(DRightButton,2,1,1,1);
    DControllerGrid->Attach(DDownButton,1,2,1,1);
    DControllerGrid->Attach(DCommandButton,3,1,1,1);
    DControllerGrid->Attach(DButton1,4,0,1,1);
    DControllerGrid->Attach(DButton2,6,0,1,1);
    DControllerGrid->Attach(DButton3,4,2,1,1);
    DControllerGrid->Attach(DButton4,6,2,1,1);
    DControllerGrid->SetRowSpacing(5);
    DControllerGrid->SetColumnSpacing(5);


    DKeyControllerMapping[SGUIKeyType::KeyW] = DUpButton;
    DKeyControllerMapping[SGUIKeyType::Keyw] = DUpButton;
    DUpButton->SetLabel("w");
    DUpButton->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DUpButton->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyX] = DDownButton;
    DKeyControllerMapping[SGUIKeyType::Keyx] = DDownButton;
    DDownButton->SetLabel("x");
    DDownButton->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DDownButton->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyA] = DLeftButton;
    DKeyControllerMapping[SGUIKeyType::Keya] = DLeftButton;
    DLeftButton->SetLabel("a");
    DLeftButton->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DLeftButton->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyD] = DRightButton;
    DKeyControllerMapping[SGUIKeyType::Keyd] = DRightButton;
    DRightButton->SetLabel("d");
    DRightButton->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DRightButton->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyU] = DButton1;
    DKeyControllerMapping[SGUIKeyType::Keyu] = DButton1;
    DButton1->SetLabel("u");
    DButton1->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DButton1->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyI] = DButton2;
    DKeyControllerMapping[SGUIKeyType::Keyi] = DButton2;
    DButton2->SetLabel("i");
    DButton2->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DButton2->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyJ] = DButton3;
    DKeyControllerMapping[SGUIKeyType::Keyj] = DButton3;
    DButton3->SetLabel("j");
    DButton3->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DButton3->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DKeyControllerMapping[SGUIKeyType::KeyK] = DButton4;
    DKeyControllerMapping[SGUIKeyType::Keyk] = DButton4;
    DButton4->SetLabel("k");
    DButton4->SetButtonPressEventCallback(this, ControllerButtonClickEventCallback);
    DButton4->SetButtonReleaseEventCallback(this, ControllerButtonClickEventCallback);

    DCommandButton->SetLabel("CMD");
    DCommandButton->SetButtonPressEventCallback(this,CommandButtonClickEventCallback);

    DDirectionButtonMapping[DUpButton] = CRISCVConsole::EDirection::Up;
    DDirectionButtonMapping[DDownButton] = CRISCVConsole::EDirection::Down;
    DDirectionButtonMapping[DLeftButton] = CRISCVConsole::EDirection::Left;
    DDirectionButtonMapping[DRightButton] = CRISCVConsole::EDirection::Right;

    DButtonNumberButtonMapping[DButton1] = CRISCVConsole::EButtonNumber::Button1;
    DButtonNumberButtonMapping[DButton2] = CRISCVConsole::EButtonNumber::Button2;
    DButtonNumberButtonMapping[DButton3] = CRISCVConsole::EButtonNumber::Button3;
    DButtonNumberButtonMapping[DButton4] = CRISCVConsole::EButtonNumber::Button4;
}

void CRISCVConsoleApplication::CreateSystemControlWidgets(){
    DPowerButton = CGUIFactory::NewToggleButton();
    DResetButton = CGUIFactory::NewButton();
    DFirmwareButton = CGUIFactory::NewButton();
    DCartridgeButton = CGUIFactory::NewButton();
    DEjectButton = CGUIFactory::NewButton();
    DSystemControlGrid = CGUIFactory::NewGrid();
    DSystemControlGrid->Attach(DCartridgeButton,0,0,1,1);
    DSystemControlGrid->Attach(DEjectButton,1,0,1,1);
    DSystemControlGrid->Attach(DResetButton,0,1,1,1);
    DSystemControlGrid->Attach(DPowerButton,1,1,1,1);
    DSystemControlGrid->Attach(DFirmwareButton,0,2,2,1);
    DSystemControlGrid->SetRowSpacing(5);
    DSystemControlGrid->SetColumnSpacing(5);

    DPowerButton->SetLabel("PWR");
    DPowerButton->SetToggledEventCallback(this, PowerButtonToggledEventCallback);

    DResetButton->SetLabel("RST");
    DResetButton->SetButtonPressEventCallback(this,ResetButtonClickEventCallback);

    DFirmwareButton->SetLabel("Firmware");
    DFirmwareButton->SetButtonPressEventCallback(this,FirmwareButtonClickEventCallback);

    DCartridgeButton->SetLabel("INS");
    DCartridgeButton->SetButtonPressEventCallback(this,CartridgeButtonClickEventCallback);

    DEjectButton->SetLabel("REM");
    DEjectButton->SetButtonPressEventCallback(this,EjectButtonClickEventCallback);
}

void CRISCVConsoleApplication::CreateDebugWidgets(){
    DConsoleDebugBox = CGUIFactory::NewBox(CGUIBox::EOrientation::Horizontal,5);
    DDebugBox = CGUIFactory::NewBox(CGUIBox::EOrientation::Vertical,5);
    CreateDebugRegisterWidgets();
    DDebugBox->PackStart(DRegisterGrid,false,false,4);

    CreateDebugControlWidgets();
    DDebugBox->PackStart(DDebugControlBox,false,false,4);

    CreateDebugInstructionWidgets();
    CreateDebugCSRWidgets();
    auto InstCSRGrid = CGUIFactory::NewGrid();
    auto InstLabel = CGUIFactory::NewLabel("Instructions");
    auto CSRLabel = CGUIFactory::NewLabel("CSRs");
    InstLabel->SetJustification(SGUIJustificationType::Left);
    CSRLabel->SetJustification(SGUIJustificationType::Left);
    InstCSRGrid->Attach(InstLabel,0,0,1,1);
    InstCSRGrid->Attach(DDebugInstructions->ContainingWidget(),0,1,1,1);
    InstCSRGrid->Attach(CSRLabel,1,0,1,1);
    InstCSRGrid->Attach(DDebugCSRegisters->ContainingWidget(),1,1,1,1);
    InstCSRGrid->SetColumnSpacing(5);
    DDebugBox->PackStart(InstCSRGrid,false,false,4);
    CreateDebugMemoryWidgets();
    auto MemoryGrid = CGUIFactory::NewGrid();
    auto MemoryLabel = CGUIFactory::NewLabel("Memory");
    DDebugMemoryFirmwareButton = CGUIFactory::NewButton();
    DDebugMemoryCartridgeButton = CGUIFactory::NewButton();
    DDebugMemoryRegistersButton = CGUIFactory::NewButton();
    DDebugMemoryVideoButton = CGUIFactory::NewButton();
    DDebugMemoryDataButton = CGUIFactory::NewButton();
    DDebugMemoryStackButton = CGUIFactory::NewToggleButton();
    MemoryLabel->SetJustification(SGUIJustificationType::Left);
    MemoryGrid->Attach(MemoryLabel,0,0,1,1);
    MemoryGrid->Attach(DDebugMemoryFirmwareButton,1,0,1,1);
    MemoryGrid->Attach(DDebugMemoryCartridgeButton,2,0,1,1);
    MemoryGrid->Attach(DDebugMemoryRegistersButton,3,0,1,1);
    MemoryGrid->Attach(DDebugMemoryVideoButton,4,0,1,1);
    MemoryGrid->Attach(DDebugMemoryDataButton,5,0,1,1);
    MemoryGrid->Attach(DDebugMemoryStackButton,6,0,1,1);
    MemoryGrid->Attach(DDebugMemory->ContainingWidget(),0,1,8,1);
    MemoryGrid->SetRowSpacing(5);
    MemoryGrid->SetColumnSpacing(5);
    DDebugBox->PackStart(MemoryGrid,false,false,4);

    DDebugMemoryFirmwareButton->SetLabel("FW");
    DDebugMemoryFirmwareButton->SetButtonPressEventCallback(this,DebugMemoryButtonClickEventCallback);
    DDebugMemoryButtonMapping[DDebugMemoryFirmwareButton] = DRISCVConsole->FirmwareMemoryBase();
    DDebugMemoryCartridgeButton->SetLabel("CTR");
    DDebugMemoryCartridgeButton->SetButtonPressEventCallback(this,DebugMemoryButtonClickEventCallback);
    DDebugMemoryButtonMapping[DDebugMemoryCartridgeButton] = DRISCVConsole->CartridgeMemoryBase();
    DDebugMemoryRegistersButton->SetLabel("CS");
    DDebugMemoryRegistersButton->SetButtonPressEventCallback(this,DebugMemoryButtonClickEventCallback);
    DDebugMemoryButtonMapping[DDebugMemoryRegistersButton] = DRISCVConsole->RegisterMemoryBase();
    DDebugMemoryVideoButton->SetLabel("VID");
    DDebugMemoryVideoButton->SetButtonPressEventCallback(this,DebugMemoryButtonClickEventCallback);
    DDebugMemoryButtonMapping[DDebugMemoryVideoButton] = DRISCVConsole->VideoMemoryBase();
    DDebugMemoryDataButton->SetLabel("GP");
    DDebugMemoryDataButton->SetButtonPressEventCallback(this,DebugMemoryButtonClickEventCallback);
    DDebugMemoryStackButton->SetLabel("SP");
    DDebugMemoryStackButton->SetToggledEventCallback(this,DebugMemoryStackButtonToggledEventCallback);
    

    DConsoleDebugBox->PackStart(DConsoleBox,false,false,4);
    DConsoleDebugBox->PackStart(DDebugBox,false,false,4);
}

void CRISCVConsoleApplication::CreateDebugRegisterWidgets(){
    DRegisterGrid = CGUIFactory::NewGrid(); 
    auto QuarterCount = CRISCVCPU::RegisterCount() / 4;
    size_t MaxChar = 0;
    for(int RegisterIndex = 0; RegisterIndex < CRISCVCPU::RegisterCount(); RegisterIndex++){
        MaxChar = std::max(CRISCVCPU::CInstruction::RegisterName(RegisterIndex).length(),MaxChar);
    }
    MaxChar++;
    auto Title = CGUIFactory::NewLabel("CPU Regs");
    Title->SetJustification(SGUIJustificationType::Left);
    DRegisterGrid->Attach(Title,0,0,8,1);
    for(int RegisterIndex = 0; RegisterIndex < CRISCVCPU::RegisterCount(); RegisterIndex++){
        auto RegisterName = CRISCVCPU::CInstruction::RegisterName(RegisterIndex);
        if(RegisterName == "gp"){
            DDebugMemoryGlobalPointerRegisterIndex = RegisterIndex;
        }
        else if(RegisterName == "sp"){
            DDebugMemoryStackPointerRegisterIndex = RegisterIndex;
        }
        DGeneralRegisterNameLabels.push_back(CGUIFactory::NewLabel(RegisterName + ":"));
        DGeneralRegisterNameLabels.back()->SetJustification(SGUIJustificationType::Right);
        DGeneralRegisterNameLabels.back()->SetHorizontalExpand(true);
        DGeneralRegisterNameLabels.back()->SetFontFamily("monospace");
        DGeneralRegisterNameLabels.back()->SetWidthCharacters(MaxChar);
        DRegisterGrid->Attach(DGeneralRegisterNameLabels.back(),(RegisterIndex/QuarterCount)*2,(RegisterIndex%QuarterCount)+1,1,1);
        DGeneralRegisterValueLabels.push_back(CGUIFactory::NewLabel("XXXXXXXX"));
        DGeneralRegisterValueLabels.back()->SetFontFamily("monospace");
        DGeneralRegisterValueLabels.back()->SetWidthCharacters(8);
        DRegisterGrid->Attach(DGeneralRegisterValueLabels.back(),(RegisterIndex/QuarterCount)*2+1,(RegisterIndex%QuarterCount)+1,1,1);
    }
    DProgramCounterNameLabel = CGUIFactory::NewLabel("pc:");
    DProgramCounterNameLabel->SetJustification(SGUIJustificationType::Right);
    DProgramCounterNameLabel->SetHorizontalExpand(true);
    DProgramCounterNameLabel->SetFontFamily("monospace");
    DProgramCounterNameLabel->SetWidthCharacters(MaxChar);
    DRegisterGrid->Attach(DProgramCounterNameLabel,0,QuarterCount+1,1,1);
    DProgramCounterValueLabel = CGUIFactory::NewLabel("XXXXXXXX");
    DProgramCounterValueLabel->SetFontFamily("monospace");
    DProgramCounterValueLabel->SetWidthCharacters(8);
    DRegisterGrid->Attach(DProgramCounterValueLabel,1,QuarterCount+1,1,1);

    DRegisterGrid->SetHorizontalExpand(false);
}

void CRISCVConsoleApplication::CreateDebugControlWidgets(){
    DDebugControlBox = CGUIFactory::NewBox(CGUIBox::EOrientation::Horizontal,5);

    DDebugRunButton = CGUIFactory::NewToggleButton();
    DDebugStepButton = CGUIFactory::NewButton();
    DDebugRunButton->SetLabel("Run");
    DDebugStepButton->SetLabel("Step");
    DDebugControlBox->PackStart(DDebugRunButton,false,false,4);
    DDebugControlBox->PackStart(DDebugStepButton,false,false,4);

}

void CRISCVConsoleApplication::CreateDebugInstructionWidgets(){
    DDebugInstructions = std::make_shared<CGUIScrollableLabelBox>();
    // Assume @01234567: abcdef zero,zero,-2147483648
    DDebugInstructions->SetWidthCharacters(38);

}

void CRISCVConsoleApplication::CreateDebugCSRWidgets(){
    DDebugCSRegisters = std::make_shared<CGUIScrollableLabelBox>();
    // Assume mcounteren: 01234567
    DDebugCSRegisters->SetWidthCharacters(20);
    std::vector< std::string > InitialCSR;
    size_t MaxLen = 0;
    for(auto CSRAddr : DRISCVConsole->CPU()->ControlStatusRegisterKeys()){
        InitialCSR.push_back(DRISCVConsole->CPU()->ControlStatusRegisterName(CSRAddr) + std::string(": XXXXXXXX"));
        MaxLen = std::max(MaxLen,InitialCSR.back().length());
    }
    for(auto &Line : InitialCSR){
        if(Line.length() < MaxLen){
            Line = std::string(MaxLen - Line.length(),' ') + Line;
        }
    }

    DDebugCSRegisters->SetBufferedLines(InitialCSR);
}

void CRISCVConsoleApplication::CreateDebugMemoryWidgets(){
    std::unordered_map< uint32_t, uint32_t > MemoryRegions = {
        {DRISCVConsole->FirmwareMemoryBase(), DRISCVConsole->FirmwareMemorySize()},
        {DRISCVConsole->CartridgeMemoryBase(), DRISCVConsole->CartridgeMemorySize()},
        {DRISCVConsole->RegisterMemoryBase(), DRISCVConsole->RegisterMemorySize()},
        {DRISCVConsole->VideoMemoryBase(), DRISCVConsole->VideoMemorySize()},
        {DRISCVConsole->MainMemoryBase(), DRISCVConsole->MainMemorySize()}
    };
    DDebugMemory = std::make_shared<CGUIScrollableMemoryLabelBox>(DRISCVConsole->Memory(), MemoryRegions);

}

void CRISCVConsoleApplication::RefreshDebugRegisters(){
    for(size_t Index = 0; Index < CRISCVCPU::RegisterCount(); Index++){
        DGeneralRegisterValueLabels[Index]->SetText(FormatHex32Bit(DRISCVConsole->CPU()->Register(Index)));
    }
    auto PC = DRISCVConsole->CPU()->ProgramCounter();
    DProgramCounterValueLabel->SetText(FormatHex32Bit(PC));
    size_t LineIndex = 0;
    for(auto CSRAddr : DRISCVConsole->CPU()->ControlStatusRegisterKeys()){
        auto NewLine = DDebugCSRegisters->GetBufferedLine(LineIndex);
        NewLine.replace(NewLine.length() - 8, 8, FormatHex32Bit(DRISCVConsole->CPU()->ControlStatusRegister(CSRAddr)));
        DDebugCSRegisters->UpdateBufferedLine(LineIndex,NewLine);
        LineIndex++;
    }
    if(DDebugMemoryStackButton->GetActive()){
        auto RegisterBase = DRISCVConsole->CPU()->Register(DDebugMemoryStackPointerRegisterIndex);
        DDebugMemory->SetBaseAddress(RegisterBase,false);
    }
    DDebugMemory->Refresh();
    LineIndex = DRISCVConsole->InstructionAddressesToIndices(PC);
    DDebugInstructions->HighlightBufferedLine(LineIndex);
    if(DFollowingInstruction){
        if((DDebugInstructions->GetBaseLine() > LineIndex)||(DDebugInstructions->GetBaseLine() + DDebugInstructions->GetLineCount()/2 <= LineIndex)){
            DDebugInstructions->SetBaseLine(LineIndex < DDebugInstructions->GetLineCount()/2 ? 0 : LineIndex - (DDebugInstructions->GetLineCount()/2 - 1));
        }
    }
}

void CRISCVConsoleApplication::ParseArguments(int &argc, char *argv[]){
    for(int Index = 1; Index < argc; ){
        if(std::string("-d") == argv[Index]){
            DDebugMode = true;
            for(int OIndex = Index+1; OIndex < argc; OIndex++){
                argv[OIndex-1] = argv[OIndex];
            }
            argc--;
        }
        else{
            Index++;
        }
    }
}

std::string CRISCVConsoleApplication::FormatHex32Bit(uint32_t val){
    std::stringstream Stream;
    Stream<<std::setfill('0') << std::setw(8) << std::hex << val;
    return Stream.str();
}

int CRISCVConsoleApplication::Run(int argc, char *argv[]){
    ParseArguments(argc,argv);
    return DApplication->Run(argc, argv);
}