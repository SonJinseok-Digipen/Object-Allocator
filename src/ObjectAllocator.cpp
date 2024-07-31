#include "ObjectAllocator.h"

// https://distance3.sg.digipen.edu/2023sg-spring/pluginfile.php/54761/mod_resource/content/1/ObjectAllocator/azrael.digipen.edu/~mmead/www/Courses/CS280/ObjectAllocator-1.html
size_t ObjectAllocator::Calculate_PageSize(size_t ObjectSize, const OAConfig &config)
{
    size_t ObjectPerPage_ = config.ObjectsPerPage_;
    size_t Total_Object_Size = ObjectSize * ObjectPerPage_;
    size_t Total_Node_Size = sizeof(GenericObject);
    size_t Total_Page_Size = Total_Node_Size + Total_Page_Size;
}

void ObjectAllocator::AllocatePage()
{
    if (PageList_ == nullptr)
    {
        // 아예 완전히 처음할당해야하는상태
        char *block = new char[PageSize_];
        PageList_ = reinterpret_cast<GenericObject *>(block);
        FreeList_ = reinterpret_cast<GenericObject *>(reinterpret_cast<char *>(block) + sizeof(GenericObject));
        for (size_t i = 0; i < Stats_.ObjectSize_-1; i++)
        {
            void*block=reinterpret_cast<char*>(FreeList_)+Stats_.ObjectSize_;
            GenericObject*temp=reinterpret_cast<GenericObject*>(block);
            temp->Next=FreeList_;
            FreeList_=temp;
        }
    }
}

ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config)
{
    Config_ = config; // 인자로 들어온 config를 복사하고 싶음
    Stats_.ObjectSize_ = ObjectSize;
    size_t PageSize = Calculate_PageSize(Stats_.ObjectSize_, Config_);
}