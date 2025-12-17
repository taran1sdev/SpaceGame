#include "ddsLoader.hpp"
#include <fstream>
#include <vector>
#include <iostream>

#pragma pack(push, 1)
struct DDSHeader {
    uint32_t magic;
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch;
    uint32_t depth;
    uint32_t mipMapCount;
    uint32_t reserved1[11];

    struct {
        uint32_t size;
        uint32_t flags;
        uint32_t fourCC;
        uint32_t rgbBitCount;
        uint32_t rMask;
        uint32_t gMask;
        uint32_t bMask;
        uint32_t aMask;        
    } pixelFormat;

    struct {
        uint32_t caps1;
        uint32_t caps2;
        uint32_t caps3;
        uint32_t caps4;
    } caps;

    uint32_t reserved2;
};
#pragma pack(pop)

static bool isCubemap(const DDSHeader& header) {
    constexpr uint32_t DDSCAPS2_CUBEMAP = 0x200;
    return (header.caps.caps2 & DDSCAPS2_CUBEMAP) != 0;
}

struct DDSFormatInfo {
    GLenum internalFormat;
    GLenum externalFormat;
    GLenum type;
};

static bool getFormat(const DDSHeader& header, DDSFormatInfo& out) {
    if (header.pixelFormat.flags & 0x4) {
        if (header.pixelFormat.fourCC == 0x71) {
            out.internalFormat = GL_RGBA16F;
            out.externalFormat = GL_RGBA;
            out.type = GL_HALF_FLOAT;
            return true;
        } 
    }
    
    if (header.pixelFormat.fourCC == 0 &&
        header.pixelFormat.rgbBitCount == 32 &&
        header.pixelFormat.rMask == 0x00FF0000 &&
        header.pixelFormat.gMask == 0x0000FF00 &&
        header.pixelFormat.bMask == 0x000000FF &&
        header.pixelFormat.aMask == 0xFF000000)
    {
        out.internalFormat = GL_RGBA8;
        out.externalFormat = GL_RGBA;
        out.type           = GL_UNSIGNED_BYTE;
        return true;
    }

    if (header.pixelFormat.fourCC == 0 &&
        header.pixelFormat.rgbBitCount == 64)
    {
        out.internalFormat = GL_RGBA16F;
        out.externalFormat = GL_RGBA;
        out.type           = GL_HALF_FLOAT;
        return true;
    }
    
    std::cerr << "Unsupported DDS format\n";
    return false;
}

static bool readBytes(std::ifstream& f, void* dst, size_t count) {
    return static_cast<bool>(f.read(reinterpret_cast<char*>(dst), count));
}

GLuint loadDDSCubemap(const std::string& filePath) {
    std::ifstream f(filePath, std::ios::binary);
    if (!f.is_open()) {
        std::cerr << "Failed to open DDS file\n";
        return 0;
    }
    
    DDSHeader header{};
    if (!readBytes(f, &header, sizeof(header))) {
        std::cerr << "Failed to read DDS header\n";
        return 0;
    }

    if (header.magic != 0x20534444) {
        std::cerr << "Invalid DDS Magic Bytes\n";
        return 0;
    }

    if (!isCubemap(header)) {
        std::cerr << "DDS is not a cubmap\n";
        return 0;
    }

    DDSFormatInfo fmt{};
    if (!getFormat(header, fmt)) {
        return 0;
    }

    uint32_t width = header.width;
    uint32_t height = header.height;
    uint32_t mipCount = header.mipMapCount ? header.mipMapCount : 1;
    
    size_t bytesPerTexel = header.pixelFormat.rgbBitCount / 8;

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, 
            mipCount > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    const int faces = 6;

    for (uint32_t mip = 0; mip < mipCount; ++mip) {
        uint32_t w = std::max(1u, width >> mip);
        uint32_t h = std::max(1u, height >> mip);
        size_t faceSize = w * h * bytesPerTexel;

        for (int face = 0; face < faces; ++face) {
            std::vector<char> buffer(faceSize);
            if (!readBytes(f, buffer.data(), faceSize)) {
                std::cerr << "Error reading DDS face\n";
                glDeleteTextures(1, &texID);
                return 0;
            }
        

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                mip, fmt.internalFormat, w, h, 0, fmt.externalFormat,
                (fmt.internalFormat == GL_RGBA16F) ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE,
                buffer.data()
            ); 
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return texID;
}






























