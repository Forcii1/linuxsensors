#include <charconv>
#include <chrono>
#include <ostream>
#include <thread>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> 
#include <sstream>


long readfile(std::string path ){
    std::ifstream myfile;
    myfile.open(path);
    std::string a="";
    std::getline (myfile, a);
    myfile.close();
    if(a.length()==0){
        return -1;
    }
    return (stol(a));
} 

std::string repeatchar(std::string a){
return std::string (8-a.length(),' ');
}

int cpufreq(){
unsigned int nthreads = std::thread::hardware_concurrency();
int k=0;
int l=0;
k=readfile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
for(int i=1;i<nthreads;i++){
    l=readfile("/sys/devices/system/cpu/cpu"+std::to_string(i)+"/cpufreq/scaling_cur_freq");
    if(l>k){
        k=l;
    }
}
return k;
}

std::string gettime(int argc, char **argv){
    if(argc<2){
        return "1.0";
    }
    return argv[1];
}

std::string getramusage(int k){
    std::ifstream myfile;
    myfile.open("/proc/meminfo");
    std::string a="";
    std::getline (myfile, a);

    int test1= stoi(a.substr(a.length()-11,8));
    
    std::getline (myfile, a);

    std::getline (myfile, a);

    int test2= stoi(a.substr(a.length()-11,8));

    if(k){
        return std::to_string((((test1-test2)*100/test1)));
    }

    float test3=((test1-test2)/10000)/100.0;

    std::ostringstream ss;
    ss << test3;
    std::string s(ss.str());
    myfile.close();

    return s;
}
int main(int argc, char **argv){
    float TIME=stof(gettime(argc, argv));
    
    if(getuid()){
        std::cout<<"\nProgramm must run as ROOT! Please restart as ROOT.\n\n";
        return 0;
    }
    system("printf '\033c'");
    cpufreq();
    while (true) {
        long T0=(readfile("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj"));
        int sleep=TIME*1000;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        long T1=(readfile("/sys/class/powercap/intel-rapl/intel-rapl:0/energy_uj"));

        //------GPU------
        std::string PATH="/sys/class/hwmon/hwmon1/";
        //GPU Core Clock
        int GCF=readfile(PATH+"freq1_input")/1000000;
        //GPU memclocl
        int GCM=readfile(PATH+"freq2_input")/1000000;
        //GPU Fanspeed
        int GFS=readfile(PATH+"fan1_target");
        //GPU Temp Edge
        int GTE=readfile(PATH+"temp1_input")/1000;
        //GPU Temp junc
        int GTJ=readfile(PATH+"temp2_input")/1000;
        //GPU Temp Mem
        int GTM=readfile(PATH+"temp3_input")/1000;
        //GPU Power
        int GPO= readfile(PATH+"power1_average")/1000000;
        //GPU Volate
        int GV=readfile(PATH+"in0_input");
        //GPUVRAM

        float GVU=int(readfile("/sys/class/drm/card1/device/mem_info_vram_used")/10000000.0)/100.0;
        std::ostringstream temp2;
        temp2<<GVU; 
        std::string GVUs=temp2.str(); 
        float GVG=int(readfile("/sys/class/drm/card1/device/mem_info_vram_total")/10000000.0)/100.0;
        GVG=int((GVU/GVG)*100);
        

        //------MAINBOARD------
        PATH="/sys/class/hwmon/hwmon5/";
        int FAN1=readfile(PATH+"fan1_input");
        int FAN2=readfile(PATH+"fan2_input");
        int FAN3=readfile(PATH+"fan3_input");
        int FAN4=readfile(PATH+"fan4_input");  
        std::string RAM=getramusage(0);
        std::string RAMp=getramusage(1);
        //------CPU------
        PATH="/sys/class/hwmon/hwmon2/";
        int CPUT1= readfile(PATH+"temp1_input")/1000;
        int CPUT2= readfile(PATH+"temp3_input")/1000;

        //Cpu Power
        int CPUP=(T1-T0)/TIME/1000000;

        //CPU FREQ
        int CPUF=cpufreq()/1000;




        //------OUTPUT------

        system("printf '\033c'");
        std::cout<<"Time: "<<TIME<<" s\n\n";

        std::cout<<"GPU stats:\n\n";
        std::cout<<"Core Freq:  "<<repeatchar(std::to_string(GCF))<<GCF<<" MHz\n";
        std::cout<<"Mem Freq:   "<<repeatchar(std::to_string(GCM))<<GCM<<" MHz\n";
        std::cout<<"GPU Fan:    "<<repeatchar(std::to_string(GFS))<<GFS<<" RPM\n";
        std::cout<<"Core Volt:  "<<repeatchar(std::to_string(GV))<<GV<<" mV\n";
        std::cout<<"PPT:        "<<repeatchar(std::to_string(GPO))<<GPO<<" W\n";
        std::cout<<"Edge:       "<<repeatchar(std::to_string(GTE))<<GTE<<" °C\n";
        std::cout<<"Junc:       "<<repeatchar(std::to_string(GTJ))<<GTJ<<" °C\n";
        std::cout<<"Mem:        "<<repeatchar(std::to_string(GTM))<<GTM<<" °C\n";
        //da die Funktion mit Float nicht funktioniert, da diese immer 8 Stellen haben. Für die Länge nehme ich temp und füge eine Stelle hinzu, da der "." in Temp ignoriert wird
        if(GVG<=60){//if for color
            std::cout<<"VRAM:       "<<repeatchar(GVUs)<<GVU<<" GB     (\033[0;32m"<<int(GVG)<<"%\033[0m)\n";
        }else if(GVG<=85){
            std::cout<<"VRAM:       "<<repeatchar(GVUs)<<GVU<<" GB     (\033[0;33m"<<int(GVG)<<"%\033[0m)\n";
        }else if(GVG<=100){
            std::cout<<"VRAM:       "<<repeatchar(GVUs)<<GVU<<" GB     (\031[0;33m"<<int(GVG)<<"%\033[0m)\n";
        }
        
        std::cout<<std::endl<<std::endl;

        std::cout<<"MB stats:\n\n";
        std::cout<<"CPU-Fan:    "<<repeatchar(std::to_string(FAN1))<<FAN1<<" RPM\n";
        std::cout<<"Top Intake: "<<repeatchar(std::to_string(FAN2))<<FAN2<<" RPM\n";
        std::cout<<"Down Intake:"<<repeatchar(std::to_string(FAN4))<<FAN4<<" RPM\n";
        std::cout<<"All Outtake:"<<repeatchar(std::to_string(FAN3))<<FAN3<<" RPM\n";
        
        if(stoi(RAMp)<=60){
            std::cout<<"Ram Usage:  "<<repeatchar(RAM)<<RAM<<" GB     (\033[0;32m"<<RAMp<<"%\033[0m)\n";
        }else if(stoi(RAMp)<=85){
            std::cout<<"Ram Usage:  "<<repeatchar(RAM)<<RAM<<" GB     (\033[0;33m"<<RAMp<<"%\033[0m)\n";
        }else if(stoi(RAMp)<=100){
            std::cout<<"Ram Usage:  "<<repeatchar(RAM)<<RAM<<" GB     (\033[0;31m"<<RAMp<<"%\033[0m)\n";
        }
        std::cout<<std::endl<<std::endl;
        
        std::cout<<"CPU stats:\n\n";
        std::cout<<"CPU PPT     "<<repeatchar(std::to_string(CPUP))<<CPUP<<" W\n";
        std::cout<<"CPU Freq    "<<repeatchar(std::to_string(CPUF))<<CPUF<<" MHz\n";
        std::cout<<"CPU Temp1   "<<repeatchar(std::to_string(CPUT1))<<CPUT1<<" °C\n";
        std::cout<<"CPU Temp2   "<<repeatchar(std::to_string(CPUT2))<<CPUT2<<" °C\n";
        


    }
}