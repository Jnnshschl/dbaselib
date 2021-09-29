# dbaselib

C++ library to modify DBASE III/IV files with a special focus on performance (not safety). Stuff is made to be used from a C# Application and may contain errors.

⚠️ This library does not check whether the field type is correct, make sure to double check what you are doing! For example when you want to copy stuff, use the Copy function for text and the CopyR function for Numbers otherwise your file may become broken.

# Samples

```c++
DBase* dbase = DBaseUtils::FromFile("C:\\test.DBF");

if (dbase && dbase->Load())
{
    const auto handle = dbase->Select("SAMPLEFIELD");

    for (size_t i = 0; i < dbase->RecordCount(); ++i)
    {
        handle->SetText(i, "Hello from dbaselib!");
    }
    
    handle->SetText(0, "Sample Text...");
    
    const auto handle = dbase->Select("IMPORTANTSTUFF");
    handle->SetFloat(0, 0.0f);
}

dbase->Save("C:\\test_modified.DBF");
delete dbase;
```

# Credits

❤️ https://github.com/fastfloat/fast_float
