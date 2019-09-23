#pragma once
#include<string>
#include<assert.h>

/*!
 * ��(�X�y�[�X�C�^�u)���폜
 * @param[inout] buf ����������
 */
static void deleteSpace( std::string *buf )
{
    size_t pos;
    while((pos = buf->find_first_of(" �@\t")) != std::string::npos){
        buf->erase(pos, 1);
    }
}


/*!
 * ��(�X�y�[�X�C�^�u),���s���폜
 * @param[inout] buf ����������
 */
static void deleteExtraChars( std::string *buf )
{
    size_t pos;
    while((pos = buf->find_first_of(" �@\t\n\r")) != std::string::npos){
        buf->erase(pos, 1);
    }
}
