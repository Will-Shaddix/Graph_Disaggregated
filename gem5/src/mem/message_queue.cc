#include "mem/message_queue.hh"
// /src/mem/message_queue.hh

namespace gem5{

    // temporarily here, should be moved to a header file
    // should potentially change to std::tuple<uint32_t, uint_32_t, uint_32_t>
// struct update_item{

//     uint32_t src_id;
//     uint32_t dst_id;
//     uint32_t dist;

// };

// fake_port(name() + ".mem_side", this),

    MessageQueue::MessageQueue(const MessageQueueParams &p) :
        ClockedObject(p), queueSize(p.queueSize), my_range(p.my_range), corePorts(name() + ".cpu_side", this), fake_port(name() + ".mem_side", this),  nextReadEvent([this]{ processNextReadEvent(); }, name()),
        nextWriteEvent([this] { processNextWriteEvent(); }, name())
        {

            DPRINTF(MessageQueue, "%s: port name: %s\n", __func__,  (name() + ".cpu_side"));
            // for(int i = 0; i < p.port_connection_count; ++i){
            //     corePorts.emplace_back(name() + csprintf(".cpu_side[%d]", i), this, i);
            // }
            
                //corePorts = RespPort(name() + ".cpu_side", this);
            
        }

    bool
    MessageQueue::recvTimingReq(PacketPtr pkt)
    {
        //Addr this_addr = pkt->getAddr();
        DPRINTF(MessageQueue, "%s: isWrite(): %d, Message Queue size = %d\n", __func__,  pkt->isWrite(), queueLength());

        if(pkt->isWrite()){
            //update_item this_update = pkt->getLE<update_item>(); // data type in <> should be whatever format the update is
            // uint32_t this_update = pkt->getLE<uint32_t>();
            //uint32_t this_update = pkt->getLE<uint32_t>();

            // std::tuple<uint32_t, uint32_t, uint32_t> this_update = pkt->getLE<std::tuple<uint32_t, uint32_t, uint32_t>>(); // data type in <> should be whatever format the update is
            // DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, std::get<0>(this_update));


            // uncomment line below for uint32_t
            uint32_t this_update = pkt->getLE<uint32_t>(); // data type in <> should be whatever format the update is
            DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, this_update);


            // c++ deque
            if(queueLength() >= queueSize){
                // full queue
                return false;
            }

            // c++ deque

            //queue.emplace_back(std::make_tuple(std::get<0>(this_update), std::get<1>(this_update), std::get<2>(this_update), curTick()));
            
            queue.emplace_back(std::make_tuple(this_update, this_update, this_update, curTick()));

            
            // queue.emplace_back(std::make_tuple(this_update.src_id, this_update.dst_id, this_update.dist, curTick()));
            //queue.emplace_back(std::make_tuple(this_update, this_update, this_update, curTick()));

        }

        else if(pkt->isRead()){
            //std::tuple<uint32_t, uint32_t, uint32_t, Tick> element = queue.front();

            //std::tuple<uint32_t, uint32_t, uint32_t> to_return = std::tuple<std::get<0>(element), std::get<1>(element), std::get<2>(element)>;
            if(queueLength() > 0){
                DPRINTF(MessageQueue, "%s: Read Value: %d\n", __func__, std::get<0>(queue.front()));
                // pkt->setData(std::get<0>(queue.front()) + 15);
                pkt->setLE(std::get<0>(queue.front()) + 15);
                queue.pop_front();
                DPRINTF(MessageQueue, "%s: Returned Value: %d\n", __func__, pkt->getLE<uint32_t>());
                checkRetryReq();
            }

        }

        pkt->makeResponse();
        corePorts.sendTimingResp(pkt);
        

        return true;
    }

    // Tick 
    // MessageQueue::recvAtomic(PacketPtr pkt){

    // }

    void 
    MessageQueue::recvFunctional(PacketPtr pkt){

        if(pkt->isWrite()){
            // uncomment line below for tuple
            // std::tuple<uint32_t, uint32_t, uint32_t> this_update = pkt->getLE<std::tuple<uint32_t, uint32_t, uint32_t>>(); // data type in <> should be whatever format the update is
            // DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, std::get<0>(this_update));
            
            // uncomment line below for uint32_t
            uint32_t this_update = pkt->getLE<uint32_t>(); // data type in <> should be whatever format the update is
            DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, this_update);
            
            
            //update_item this_update = pkt->getLE<update_item>(); // data type in <> should be whatever format the update is
            // uint32_t this_update = pkt->getLE<uint32_t>();
            //uint32_t this_update = pkt->getLE<uint32_t>();
            // DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, std::get<0>(this_update));

            // c++ deque
            if(queueLength() >= queueSize){
                // full queue
                
            } else{
            // c++ deque

            // uncomment for fuple
            //queue.emplace_back(std::make_tuple(std::get<0>(this_update), std::get<1>(this_update), std::get<2>(this_update), curTick()));
            
            // uncomment for uiint32_t
            queue.emplace_back(std::make_tuple(this_update, this_update, this_update, curTick()));

            // queue.emplace_back(std::make_tuple(this_update.src_id, this_update.dst_id, this_update.dist, curTick()));
            //queue.emplace_back(std::make_tuple(this_update, this_update, this_update, curTick()));
            }
        }

        else if(pkt->isRead()){
            //std::tuple<uint32_t, uint32_t, uint32_t, Tick> element = queue.front();

            //std::tuple<uint32_t, uint32_t, uint32_t> to_return = std::tuple<std::get<0>(element), std::get<1>(element), std::get<2>(element)>;
            
            if(queueLength() > 0){
                DPRINTF(MessageQueue, "%s: Read Value: %d\n", __func__, std::get<0>(queue.front()));
                // pkt->setData(std::get<0>(queue.front()) + 15);
                pkt->setLE(std::get<0>(queue.front()) + 15);
                queue.pop_front();
                DPRINTF(MessageQueue, "%s: Returned Value: %d\n", __func__, pkt->getLE<uint32_t>());
                checkRetryReq();
            }

        }

        pkt->makeResponse();
        //corePorts.sendFunctionalResp(pkt);
        

    
    
        //panic("recvFunctional unimpl.");

    }

    void
    MessageQueue::init()
    {
        corePorts.sendRangeChange();
        // fake_port.recvRangeChange();
    }


    void
    MessageQueue::processNextWriteEvent()
    {

        // std::tuple<uint32_t, uint32_t, uint32_t> this_update = pkt->getLE<std::tuple<uint32_t, uint32_t, uint32_t>>(); // data type in <> should be whatever format the update is
        //     //update_item this_update = pkt->getLE<update_item>(); // data type in <> should be whatever format the update is
        //     // uint32_t this_update = pkt->getLE<uint32_t>();
        //     //uint32_t this_update = pkt->getLE<uint32_t>();
        //     DPRINTF(MessageQueue, "%s: Write Value: %d\n", __func__, std::get<0>(this_update));

        //     // c++ deque
        //     if(queueLength() >= queueSize){
        //         // full queue should send retry
        //         needSendRetryReq = true;
                
        //     }

        //     // c++ deque

        //     queue.emplace_back(std::make_tuple(std::get<0>(this_update), std::get<1>(this_update), std::get<2>(this_update), curTick()));
            

        //     pkt->makeResponse();
        //     corePorts.sendTimingResp(pkt);
    }

    void
    MessageQueue::processNextReadEvent()
    {
            //pkt->setData() should pass in a pointer to the data, not the sata itself
            //pkt->setLE()?
            //makeResponse


        //     if(queueLength() > 0){
        //         DPRINTF(MessageQueue, "%s: Read Value: %d\n", __func__, std::get<0>(queue.front()));
        //         // pkt->setData(std::get<0>(queue.front()) + 15);
        //         pkt->setLE(std::get<0>(queue.front()) + 15); // Replace this with whatever value we want to put in there
        //         queue.pop_front();
        //         pkt->makeResponse();
        //         corePorts.sendTimingResp(pkt);
        //         DPRINTF(MessageQueue, "%s: Returned Value: %d\n", __func__, pkt->getLE<uint32_t>());
        //     }

        // pkt->makeResponse();
        // corePorts.sendTimingResp(pkt);

        // // if(corePorts.needSendRetryReq){
        // //     corePorts.needSendRetryReq = false;
        // //     corePorts.sendRetryReq();
        // // }

    }

    void
    MessageQueue::checkRetryReq()
    {
        DPRINTF(MessageQueue, "%s: checking retry:\n", __func__);
        corePorts.checkRetryReq();
    }


    AddrRangeList
    MessageQueue::RespPort::getAddrRanges() const
    {
        return owner->getAddrRanges();
    }


    void
    MessageQueue::RespPort::checkRetryReq()
    {
        DPRINTF(MessageQueue, "%s: checking retry: %d\n", __func__, needSendRetryReq);
        if (needSendRetryReq) {
            needSendRetryReq = false;
            sendRetryReq();
        }
    }

    bool
    MessageQueue::RespPort::recvTimingReq(PacketPtr pkt)
    {
        DPRINTF(MessageQueue, "%s: Port Received Request\n", __func__);
        
        if (!owner->recvTimingReq(pkt)) {
            needSendRetryReq = true;
            return false;
        }

        return true;
    }

    Tick
    MessageQueue::RespPort::recvAtomic(PacketPtr pkt)
    {
        panic("recvAtomic unimpl.");
    }

    void
    MessageQueue::RespPort::recvFunctional(PacketPtr pkt)
    {
        owner->recvFunctional(pkt);
    }

    void
    MessageQueue::RespPort::recvRespRetry()
    {
        panic("recvRespRetry from response port is called.");
    }

    Port&
    MessageQueue::getPort(const std::string& if_name, PortID idx)
    {
        if (if_name == "cpu_side" || (idx == 0)){
            return corePorts;
        }
        else if (if_name == "mem_side" || (idx == 1)){
            return fake_port;
        }
        else {
            return ClockedObject::getPort(if_name, idx);
        }
       
    }



    void
    MessageQueue::ReqPort::sendPacket(PacketPtr pkt)
    {
        panic_if(blockedPacket != nullptr,
                "Should never try to send if blocked!");
        // If we can't send the packet across the port, store it for later.
        if (!sendTimingReq(pkt))
        {
            DPRINTF(MessageQueue, "%s: Packet is blocked.\n", __func__);
            blockedPacket = pkt;
        }
    }

    bool
    MessageQueue::ReqPort::recvTimingResp(PacketPtr pkt)
    {
        panic("recvTimingResp called on the request port.");
    }

    void
    MessageQueue::ReqPort::recvReqRetry()
    {
        panic_if(blockedPacket == nullptr,
                "Received retry without a blockedPacket.");

    
        PacketPtr pkt = blockedPacket;
        blockedPacket = nullptr;
        sendPacket(pkt);
        // if (blockedPacket == nullptr) {
        //     owner->recvReqRetry();
        // }
    }

}