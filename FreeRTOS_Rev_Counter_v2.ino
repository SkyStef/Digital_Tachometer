#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include<SoftwareSerial.h>
                
SoftwareSerial mySerial(11, 10); // RX | TX

void Serial_Print_Task(void* pvParameters);
void Bluetooth_Task(void* pvParameters);
void Count_RPM_Task(void* pvParameters);

SemaphoreHandle_t xSerialSemaphore=NULL;

int RPM = 0;  
int LOW_toggle_count = 0;
char RPM_[10];

void setup()
{  
    
  Serial.begin(9600);
  mySerial.begin(9600);

  if (xSerialSemaphore == NULL)
    xSerialSemaphore = xSemaphoreCreateMutex();
  else if ((xSerialSemaphore) != NULL)
    xSemaphoreGive(xSerialSemaphore);
  
  attachInterrupt(1, Sensor_ISR, CHANGE); 
  
  xTaskCreate(Serial_Print_Task, "Serial_Print_Task", 100, NULL, 1, NULL);  
  xTaskCreate(Bluetooth_Task, "Bluetooth_Task", 100, NULL, 1, NULL);  
  xTaskCreate(Count_RPM_Task, "Count_RPM_Task", 100, NULL, 1, NULL);
   
  vTaskStartScheduler();
}

void loop()
{
}

void Serial_Print_Task(void* pvParameters)
{        
  while(1)
  {
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE)
     {
        Serial.println(RPM);
     }
     xSemaphoreGive(xSerialSemaphore);
     vTaskDelay(1);
  }
}

void Bluetooth_Task(void* pvParameters)
{        
 const TickType_t Delay = 200 / portTICK_PERIOD_MS;

  while(1)
  {
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE)
     {
        mySerial.write(RPM_);
        mySerial.write("\n");
     }
     xSemaphoreGive(xSerialSemaphore);
     vTaskDelay(Delay);
  }
}

void Count_RPM_Task(void* pvParameters)
{        
  const TickType_t Delay = 200 / portTICK_PERIOD_MS;
  
  while(1)
  {
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 10) == pdTRUE)
     {
        RPM = LOW_toggle_count * 300;
        itoa(RPM, RPM_, 10);
        LOW_toggle_count = 0;
     }
     xSemaphoreGive(xSerialSemaphore);
     vTaskDelay(Delay);
  }
}

void Sensor_ISR()
{
  LOW_toggle_count++;
}
