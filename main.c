#include "J1939_Config.H"
#include "J1939.H"

// 轮询模式简单使用示例
// 备注：接受处理，不是标准的接受处理。这里只是测试接受
void J1939_CkeckFunc(void)
{   
    can_init();   
    J1939_Initialization( TRUE );   
    //等待地址超时    
    while (J1939_Flags.WaitingForAddressClaimContention)      
    J1939_Poll(5);    //设备确认总线上没有，竞争地址的设备存在    
    while (1)
    {   
        /***********************发送数据***************************/      
        Msg.DataPage                = 0;       
        Msg.Priority                = J1939_CONTROL_PRIORITY;       
        Msg.DestinationAddress      = OTHER_NODE;       
        Msg.DataLength              = 8;       
        Msg.PDUFormat               = 0xfe;      
        Msg.Data[0]         = 0xFF;       
        Msg.Data[1]         = 0xFF;       
        Msg.Data[2]         = 0xFF;       
        Msg.Data[3]         = 0xFF;      
        Msg.Data[4]         = 0xFF;      
        Msg.Data[5]         = 0xFF;        
        Msg.Data[6]         = 0xFF;        
        Msg.Data[7]         = 0xFF;         
        while (J1939_EnqueueMessage( &Msg ) != RC_SUCCESS)            
        J1939_Poll(5);    
        /***********************处理接受数据*************************/      
        while (RXQueueCount > 0) 
        {    
            J1939_DequeueMessage( &Msg );       
            if (Msg.PDUFormat == 0x01)   
            {
                //用户定义的功能码;     
            }            
            else if (Msg.PDUFormat == 0x02)  
            {
                //用户定义的功能码;        
            }             
        }        
        J1939_Poll(20);   
    }
 }
// 中断模式
void J1939_ISR(void)
{    
    can_init();    
    J1939_Initialization( TRUE );   
    //等待地址超时    
    while (J1939_Flags.WaitingForAddressClaimContention)        
    J1939_Poll(5);    //设备确认总线上没有，竞争地址的设备存在    
    while (1)    
    {        
    //判断接受列队中，存在多少个接受消息（RXQueueCount ）       
        while (RXQueueCount > 0) 
        {//读取接受列队中的数据到Msg （出队）           
            J1939_DequeueMessage( &Msg );            /*判断是否是数据请求帧*/           
            if (Msg.PDUFormat == J1939_PF_REQUEST)           
            {                //判断参数群是否被本设备支持               
                if ((Msg.Data[0] == J1939_PGN0_REQ_ENGINE_SPEED) 
                    &&(Msg.Data[1] == J1939_PGN1_REQ_ENGINE_SPEED) 
                    &&(Msg.Data[2] == J1939_PGN2_REQ_ENGINE_SPEED))               
                {                    
                    if (某种原因不能响应)                    
                    {                       
                        /*********发送不能响应（参考J1939-21）*************/   
                        Msg.Priority            = J1939_ACK_PRIORITY;           
                        Msg.DataPage            = 0;                   
                        Msg.PDUFormat           = J1939_PF_ACKNOWLEDGMENT;      
                        Msg.DestinationAddress  = Msg.SourceAddress;               
                        Msg.DataLength          = 8;                      
                        Msg.Data[0]         = J1939_NACK_CONTROL_BYTE;     
                        Msg.Data[1]         = 0xFF;                        
                        Msg.Data[2]         = 0xFF;                        
                        Msg.Data[3]         = 0xFF;                        
                        Msg.Data[4]         = 0xFF;                        
                        Msg.Data[5]         = J1939_PGN0_REQ_ENGINE_SPEED;         
                        Msg.Data[6]         = J1939_PGN1_REQ_ENGINE_SPEED;         
                        Msg.Data[7]         = J1939_PGN2_REQ_ENGINE_SPEED;         
                    }                  
                    else               
                    {                    /*******************上传相关的参数群*****************/       
                        Msg.Priority    = J1939_INFO_PRIORITY;              
                        Msg.DataPage    = J1939_PGN2_REQ_ENGINE_SPEED & 0x01;   
                        Msg.PDUFormat   = J1939_PGN1_REQ_ENGINE_SPEED;          
                        Msg.GroupExtension = J1939_PGN0_REQ_ENGINE_SPEED;       
                        Msg.DataLength  = 1;                      
                        Msg.Data[0]     = EngineSpeed;            
                    }                  
                    while (J1939_EnqueueMessage( &Msg ) != RC_SUCCESS);      
                }           
            }       
        }   
    }
}

void main( void )
{
    J1939_ISR();
    J1939_CkeckFunc();
}