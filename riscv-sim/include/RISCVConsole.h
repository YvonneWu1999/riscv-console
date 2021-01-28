#ifndef RISCVCCONSOLE_H
#define RISCVCCONSOLE_H

#include "RISCVCPU.h"
#include "RISCVConsoleChipset.h"
#include "RegisterBlockMemoryDevice.h"
#include "ElfLoad.h"
#include "MemoryDevice.h"
#include "FlashMemoryDevice.h"
#include "VideoController.h"
#include "DataSource.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class CRISCVConsole{
    public:
        enum class EDirection : uint32_t {Left = 0x1, Up = 0x2, Down = 0x4, Right = 0x8};
        enum class EButtonNumber : uint32_t {Button1 = 0x10, Button2 = 0x20, Button3 = 0x40, Button4 = 0x80};
    protected:
        enum class EThreadState : uint32_t {Stop = 0, Run = 1, Terminate = 2};
        std::shared_ptr< CRISCVCPU > DCPU;
        std::shared_ptr< CRISCVCPU::CInstructionCache > DCPUCache;
        std::shared_ptr< CMemoryDevice > DMemoryController;
        std::shared_ptr< CMemoryDevice > DMainMemory;
        std::shared_ptr< CRegisterBlockMemoryDevice > DRegisterBlock;
        std::shared_ptr< CFlashMemoryDevice > DFirmwareFlash;
        std::shared_ptr< CFlashMemoryDevice > DCartridgeFlash;
        std::shared_ptr< CVideoController > DVideoController;
        std::shared_ptr< CRISCVConsoleChipset > DChipset;
        std::shared_ptr< CHardwareRegister< uint32_t > > DControllerState;

        std::atomic<uint32_t> DSystemCommand;
        std::atomic<uint32_t> DCPUAcknowledge;
        std::atomic<uint32_t> DTimerAcknowledge;
        std::shared_ptr< std::thread > DCPUThread;
        std::shared_ptr< std::thread > DTimerThread;
        std::mutex DCPUMutex;
        std::mutex DTimerMutex;
        std::condition_variable DCPUConditionVariable;
        std::condition_variable DTimerConditionVariable;

        std::chrono::steady_clock::time_point DSystemStartTime;
        uint64_t DCPUStartInstructionCount;
        
        std::vector< std::string > DFirmwareInstructionStrings;
        std::unordered_map< uint32_t, size_t > DFirmwareAddressesToIndices;

        std::vector< std::string > DCartridgeInstructionStrings;
        std::unordered_map< uint32_t, size_t > DCartridgeAddressesToIndices;

        std::vector< std::string > DInstructionStrings;
        std::unordered_map< uint32_t, size_t > DInstructionAddressesToIndices;


        static const uint32_t DMainMemorySize;
        static const uint32_t DMainMemoryBase;
        static const uint32_t DFirmwareMemorySize;
        static const uint32_t DFirmwareMemoryBase;
        static const uint32_t DCartridgeMemorySize;
        static const uint32_t DCartridgeMemoryBase;
        static const uint32_t DVideoMemoryBase;
        static const uint32_t DRegisterMemoryBase;

        bool SystemNotStop();
        void CPUThreadExecute();
        void TimerThreadExecute();
        void SystemRun();
        void SystemStop();
        void SystemTerminate();
        void ResetComponents();

        void ConstructInstructionStrings(CElfLoad &elffile, std::vector< std::string > &strings, std::unordered_map< uint32_t, size_t > &translations);
        void ConstructFirmwareStrings(CElfLoad &elffile);
        void ConstructCartridgeStrings(CElfLoad &elffile);

    public:
        CRISCVConsole();
        ~CRISCVConsole();

        uint32_t ScreenWidth(){
            return DVideoController->ScreenWidth();
        };

        uint32_t ScreenHeight(){
            return DVideoController->ScreenHeight();
        };

        std::shared_ptr< CRISCVCPU > CPU(){
            return DCPU;
        };

        std::shared_ptr< CMemoryDevice > Memory(){
            return DMemoryController;
        };

        void Reset();

        void PowerOn();

        void PowerOff();

        void PressDirection(EDirection dir);
        void ReleaseDirection(EDirection dir);

        void PressButton(EButtonNumber button);
        void ReleaseButton(EButtonNumber button);

        void PressCommand();

        bool VideoTimerTick(std::shared_ptr<CGraphicSurface> screensurface);

        bool ProgramFirmware(std::shared_ptr< CDataSource > elfsrc);

        bool InsertCartridge(std::shared_ptr< CDataSource > elfsrc);

        bool RemoveCartridge();

        const std::vector< std::string > &InstructionStrings() const{
            return DInstructionStrings;
        }

        size_t InstructionAddressesToIndices(uint32_t addr) const{
            auto Search = DInstructionAddressesToIndices.find(addr);
            if(Search != DInstructionAddressesToIndices.end()){
                return Search->second;
            }
            return -1;
        }
        
        uint32_t MainMemorySize(){
            return DMainMemorySize;
        };

        uint32_t MainMemoryBase(){
            return DMainMemoryBase;
        };

        uint32_t FirmwareMemorySize(){
            return DFirmwareMemorySize;
        };

        uint32_t FirmwareMemoryBase(){
            return DFirmwareMemoryBase;
        };

        uint32_t CartridgeMemorySize(){
            return DCartridgeMemorySize;
        };

        uint32_t CartridgeMemoryBase(){
            return DCartridgeMemoryBase;
        };

        uint32_t VideoMemorySize(){
            return DVideoController->VideoRAM()->MemorySize();
        };

        uint32_t VideoMemoryBase(){
            return DVideoMemoryBase;
        };

        uint32_t RegisterMemorySize(){
            return DRegisterBlock->MemorySize();
        };

        uint32_t RegisterMemoryBase(){
            return DRegisterMemoryBase;
        };

        

};

#endif
