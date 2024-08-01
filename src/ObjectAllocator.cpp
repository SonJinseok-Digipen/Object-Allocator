#include "ObjectAllocator.h"
#include <iostream>
// https://distance3.sg.digipen.edu/2023sg-spring/pluginfile.php/54761/mod_resource/content/1/ObjectAllocator/azrael.digipen.edu/~mmead/www/Courses/CS280/ObjectAllocator-1.html
size_t ObjectAllocator::Calculate_PageSize(size_t ObjectSize, const OAConfig &config)
{
    size_t ObjectPerPage_ = config.ObjectsPerPage_;
    size_t Total_Object_Size = ObjectSize * ObjectPerPage_;
    size_t Total_Node_Size = sizeof(GenericObject);
    size_t Total_Page_Size = Total_Node_Size + Total_Object_Size;
    return Total_Page_Size;
}

void ObjectAllocator::AllocatePage()
{
    if (PageList_ == nullptr)
    {
        // 아예 완전히 처음할당해야하는상태
        char *block = new char[Stats_.PageSize_];
        PageList_ = reinterpret_cast<GenericObject *>(block);
        FreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(block) + sizeof(GenericObject));
        for (size_t i = 0; i < Config_.ObjectsPerPage_ - 1; i++)
        {
            void *block = reinterpret_cast<char *>(FreeList_) + Stats_.ObjectSize_;
            GenericObject *temp = reinterpret_cast<GenericObject *>(block);
            temp->Next = FreeList_;
            FreeList_ = temp;
        }
        Stats_.FreeObjects_ += Config_.ObjectsPerPage_;
        Stats_.PagesInUse_++;
        PageList_->Next = nullptr;
    }
    else
    {

        char *block = new char[Stats_.PageSize_];
        GenericObject *NewPageList_ = reinterpret_cast<GenericObject *>(block);
        FreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(block) + sizeof(GenericObject));
        for (size_t i = 0; i < Config_.ObjectsPerPage_ - 1; i++)
        {
            void *block = reinterpret_cast<char *>(FreeList_) + Stats_.ObjectSize_;
            GenericObject *temp = reinterpret_cast<GenericObject *>(block);
            temp->Next = FreeList_;
            FreeList_ = temp;
        }
        Stats_.FreeObjects_ += Config_.ObjectsPerPage_;
        Stats_.PagesInUse_++;
    
       GenericObject*temp=PageList_;
       NewPageList_->Next=temp;
       PageList_=NewPageList_;

           
    }
}

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config)
{
    Config_ = config; // 인자로 들어온 config를 복사하고 싶음
    Stats_.ObjectSize_ = ObjectSize;
    Stats_.PageSize_ = Calculate_PageSize(Stats_.ObjectSize_, Config_);
    // std::cout << "PageSize " << Stats_.PageSize_ << '\n';
    AllocatePage();
}

ObjectAllocator::~ObjectAllocator()
{
    while (PageList_)
    {
        GenericObject *temp = PageList_->Next;
        delete[] reinterpret_cast<char *>(PageList_);
        PageList_ = temp;
    }
    FreeList_=nullptr;
}

void *ObjectAllocator::Allocate([[maybe_unused]] const char *label)
{
    void *target = FreeList_;
    return target;
}

void ObjectAllocator::Free(void *Object)
{
    if (FreeList_ == nullptr)
    {
        FreeList_ = reinterpret_cast<GenericObject *>(Object);
    }
    else
    {
        GenericObject *NewFreeList_ = reinterpret_cast<GenericObject *>(Object);
        NewFreeList_->Next = FreeList_;
        FreeList_ = NewFreeList_;
    }
}

unsigned ObjectAllocator::DumpMemoryInUse([[maybe_unused]] ObjectAllocator::DUMPCALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::ValidatePages([[maybe_unused]] ObjectAllocator::VALIDATECALLBACK fn) const
{
    return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    return 0;
}

bool ObjectAllocator::ImplementedExtraCredit()
{
    return false;
}

void ObjectAllocator::SetDebugState(bool State)
{
    if (State == true)
    {
        Config_.UseCPPMemManager_ = true;
    }
    else
    {
        Config_.UseCPPMemManager_ = false;
    }
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
    return Config_;
}

OAStats ObjectAllocator::GetStats() const
{
    return Stats_;
}
