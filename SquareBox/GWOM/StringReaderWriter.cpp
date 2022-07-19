#include "StringReaderWriter.h"
#include "DSUString_generated.h"
#include "Asset-Manager/Asset-Manager.h"

bool SquareBox::GWOM::StringReaderWriter::saveStringDataAsBinary(const std::string& file_path_, const std::string& data_)
{
    flatbuffers::FlatBufferBuilder builder(1024);
    auto complete_data = SquareBox::DSUString::CreateSquareBoxStringData(builder, builder.CreateString(data_));

    builder.Finish(complete_data);
    uint8_t* buf = builder.GetCurrentBufferPointer();
    int size = builder.GetSize();
    return SquareBox::AssetManager::IOManager::saveRawDataToFile((const char*)buf, size, file_path_);
}

bool SquareBox::GWOM::StringReaderWriter::saveLoadDataAsBinary(const std::string& file_path_, std::string& data_)
{
    std::pair<char*, int> fileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(file_path_);
    //construct a buffer object that knows its size
    char* dataBuffer = new char[fileInfo.second];
    dataBuffer = fileInfo.first;

    auto complete_data = SquareBox::DSUString::GetSquareBoxStringData(dataBuffer);
    data_=complete_data->data()->str();
    delete[] dataBuffer;
    return true;
}
