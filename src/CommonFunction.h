#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H


class CommonFunction
{
public:
    template<typename T>
    static T* ConvertToNonConstType(const void* aValue)
    {
        T* result = static_cast<T*>(const_cast<void*>(aValue));
        return result;
    }
};

#endif // COMMONFUNCTION_H
