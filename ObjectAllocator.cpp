#include "ObjectAllocator.h"
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config)
    : config_(config)
{
    stats_.ObjectSize_ = ObjectSize;
    // 인자로 들어온 ObjectSize를 stats_변수의 ObjectSize 변수에 저장

    stats_.PageSize_ = {((config_.ObjectsPerPage_) * (stats_.ObjectSize_)) + sizeof(GenericObject *)};
    // PageSize_의 크기는 (config로 들어온 Page하나당 몇개의 오브젝트가 있어야 하나*처음설정한 ObjectSize)+GenericObject*의 크기
    // 처음 만들때 빈 페이지를 하나 만들고 시작함
    AllocateNewPage();
}

ObjectAllocator::~ObjectAllocator()
{
}

void *ObjectAllocator::Allocate([[maybe_unused]] const char *label)
{

    GenericObject*TargetMemory;
    if (stats_.FreeObjects_ > 0)
    {
        //std::cout<<"Allocate Operation Not allowed !!!"<<'\n';
        TargetMemory = FreeList_;
        FreeList_ = FreeList_->Next;
        stats_.Allocations_++;
        stats_.ObjectsInUse_++;
        stats_.FreeObjects_--;
        return TargetMemory;
    }
    else if(stats_.FreeObjects_<=0 && stats_.PagesInUse_<DEFAULT_MAX_PAGES-1)
    {
        //std::cout<<"Allocate Operation Not allowed !!!"<<'\n';
        AllocateNewPage();
        TargetMemory = FreeList_;
        FreeList_ = FreeList_->Next;
        stats_.Allocations_++;
        stats_.ObjectsInUse_++;
        stats_.FreeObjects_--;
        return TargetMemory;
    }
    else
    {

      throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES,"");
    }
    
}

void ObjectAllocator::Free(void *Object)
{
    if (FreeList_ == nullptr)
    {
        FreeList_ = reinterpret_cast<GenericObject *>(Object);
    }
    if (FreeList_ != nullptr)
    {
        GenericObject *OldFreeList = FreeList_;
        FreeList_ = reinterpret_cast<GenericObject *>(Object);
        FreeList_->Next = OldFreeList;
        stats_.FreeObjects_++;
        stats_.Allocations_--;
        stats_.Deallocations_++;
        stats_.ObjectsInUse_--;
    }
}

void ObjectAllocator::AllocateNewPage()
{

    char *buffer = new char[stats_.PageSize_];
    GenericObject *OldPageList = PageList_;
    PageList_ = reinterpret_cast<GenericObject *>(buffer);
    FreeList_ = reinterpret_cast<GenericObject *>(buffer + sizeof(GenericObject *));
    for (size_t i = 0; i < config_.ObjectsPerPage_; i++)
    {
        GenericObject *OldFreeList = FreeList_;
        FreeList_ = FreeList_ + stats_.ObjectSize_;
        FreeList_->Next = OldFreeList;
    }
    PageList_->Next = OldPageList;
    stats_.PagesInUse_++;
    stats_.FreeObjects_ += config_.ObjectsPerPage_;
}

void ObjectAllocator::SetDebugState([[maybe_unused]] bool State)
{
}

const void *ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

const void *ObjectAllocator::GetPageList() const
{
    return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
    return config_;
}

OAStats ObjectAllocator::GetStats() const
{
    return stats_;
}

unsigned ::ObjectAllocator::DumpMemoryInUse([[maybe_unused]] ObjectAllocator::DUMPCALLBACK fn) const
{
    return 0;
}

unsigned ::ObjectAllocator::ValidatePages([[maybe_unused]] VALIDATECALLBACK fn) const
{
    return 0;
}