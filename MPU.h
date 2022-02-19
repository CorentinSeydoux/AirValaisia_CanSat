#include <Wire.h>

#define port Wire1

class mpu {

private :
	int WAdress;
	int RAdress;

	int configFIFO;
	int NbrSample = 6;

	int FIFOData[];
public :

	int Setup(int Address);
	void ReadFIFO();
}

int Setup(int Address){

	this->WAdress = Address;
	this->RAdress = Address + 1;

	port.beginTransmission(this->WAdress);
	//N° de registre
	port.write(0x19); //Register Sample rate
	port.write(0x04);
	port.write(0x0A);//Config
	port.write(0x18);//Gyr config : +- 2000°/s
	port.write(0x18);//Acc config : +- 16g
	port.write(0x78);//FIFO Enable
	int a = port.write(0x1D);//Master I1C Config
	port.endTransmission();
	
	port.beginTransmission(this->WAdress);
	port.write(0x6A);//Register User_Ctrl
	port.write(0x40);//Enable FIFO
	port.write(0x01);//Enable TempSensor
	port.write(0x00);

	return(a);
}

int SlaveSetup(int Adresse,int Reg,int ByteRead,int SlaveNbr,bool EN){
	port.beginTransmission(WAdress);
	//N° de registre
	port.write(0x25 + (3*SlaveNbr)); //Slave Address
	port.write(0x80 + Adresse/2);
	port.write(Reg);
	port.write(((EN & 0x1)*0x80) + 0x20 + (ByteRead & 0x0F));
	port.endTransmission();

	int b = 0;
	if(EN){b = 1;}
	this->configFIFO = (this->configFIFO + (b<<SlaveNbr)) & (b << SlaveNbr);
	int c = 0;
	this->configFIFO &= 0x0F;
	while((this->configFIFO =! 0){
		if(this->configFIFO & 0x1){
			c++;
			this->configFIFO --;
		}
		this->configFIFO /= 2;
	}
	this->NbrSample = 6 + c;
}

void ReadFIFO(){
	
	int DataAvailable;

	port.beginTransmission(this->WAdress);
	//N° de registre
	port.write(0x72);//FIFO_Count_L
	port.beginTransmission(this->RAdress);
	DataAvailable = port.read() * 256;
	DataAvailable += port.read();
	port.endTransmission();

	port.beginTransmission(this->WAdress);
	//N° de registre
	port.write(0x74);//FIFO_Data
	port.beginTransmission(this->RAdress);
	for(int idx = 0;idx < DataAvailable;idx ++){	
		FIFOData[idx] = port.read();
	}
	port.endTransmission();	
}





















