#include "texture.hpp"

Texture::Texture(GLenum texture_type, const std::string & filename, GLenum txr_idx)
    : t_texture_type(texture_type)
    , t_txr_idx(txr_idx)
{
    auto t_image = Magick::Image(filename);

    Magick::Blob blob;
    t_image.write(&blob, "RGBA");

    glGenTextures(1, &t_texture_obj);
    glBindTexture(t_texture_type, t_texture_obj);
    glTexImage2D(t_texture_type,    /* target */
                 0,                 /* level */
                 GL_RGB,            /* internal format (number of color channels */
                 t_image.columns(), /* width */
                 t_image.rows(),    /* height */
                 0,                 /* border -- depricated */
                 GL_RGBA,           /* format */
                 GL_UNSIGNED_BYTE,  /* data type of pixel */
                 blob.data());      /* data */

    /*
     * GL_TEXTURE_MIN_FILTER -- minifying 
     * GL_TEXTURE_MAG_FILTER -- magnification
     *
     * GL_LINEAR -- weighted value of texels to pixel               
     */ 
    glTexParameteri(t_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(t_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0 + t_txr_idx);
    glBindTexture(t_texture_type, t_texture_obj);
}
