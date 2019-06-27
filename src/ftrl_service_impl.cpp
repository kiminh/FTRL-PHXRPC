/* ftrl_service_impl.cpp

 Generated by phxrpc_pb2service from ftrl.proto

*/

#include "ftrl_service_impl.h"

#include "phxrpc/file.h"

#include "ftrl_server_config.h"
#include "ftrl.pb.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#define SHM_KEY 0x1234

using namespace std;

struct shmseg {
    int mode; //0: user_id ready, 1: ad_id ready, 2:feedback ready
    long long user_id;
    long long ad_id;
    bool feedback; 
};

::google::protobuf::int64 RequestAdId(::google::protobuf::int64 user_id){
	int shmid;
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
    if (shmid == -1) {
      perror("Shared memory");
      return 1;
    }

    shmseg *shmp;

    shmp = (struct shmseg*) shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        perror("Shared memory attach");
        return 1;
    }

    while(shmp->mode != -1){
        // std::cout<<"mode: "<<shmp->mode<<"\n";
        //waiting
        sleep(1);
    }
    std::cout<<"sending user_id "<<user_id<<"\n";
    shmp->user_id = (long long) user_id;

    shmp->mode = 0;
    // std::cout<<"sending user_id "<<user_id<<"\n";

    while(shmp->mode != 1){
        // std::cout<<"mode: "<<shmp->mode<<"\n";
        //waiting for result
        sleep(1);
    }

    std::cout<<"receive ad_id "<<shmp->ad_id<<"\n";

    shmp->mode = -1;
    return (::google::protobuf::int64)shmp->ad_id;



}

void SendFeedback(::google::protobuf::int64 user_id,::google::protobuf::int64 ad_id, int feedback){
	int shmid;
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
    if (shmid == -1) {
      perror("Shared memory");
    }

    shmseg *shmp;

    shmp = (struct shmseg*) shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        perror("Shared memory attach");
    }

    while(shmp->mode != -1){
        //waiting
    }
    shmp->user_id = (long long) user_id;
    shmp->ad_id = (long long) ad_id;
    shmp->feedback = feedback;
    shmp->mode = 2;

    return;
}

FTRLServiceImpl::FTRLServiceImpl(ServiceArgs_t &app_args) : args_(app_args) {
}

FTRLServiceImpl::~FTRLServiceImpl() {
}

int FTRLServiceImpl::PHXEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    resp->set_value(req.value());
    return 0;
}

int FTRLServiceImpl::FTRL(const ftrl::FTRLRequest &req, ftrl::FTRLResult *resp) {
    std::cout<<"user_id "<<req.user_id()<<"\n";
    ::google::protobuf::int64 ad_id = RequestAdId(req.user_id());//to be implemented
    resp->set_ad_id((::google::protobuf::int64) ad_id);
    return 0;
}

int FTRLServiceImpl::Feedback(const ftrl::FTRLFeedback &req, google::protobuf::Empty *resp) {
    SendFeedback(req.user_id(),req.ad_id(),req.feedback());
    return 0;
}

