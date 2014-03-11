/*
 * shell_fonts.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef __SHELLFONT_H_
#define __SHELLFONT_H_

#include <string>
//下面是编码表
//
//编码    颜色/动作
//0   重新设置属性到缺省设置
//1   设置粗体
//2   设置一半亮度（模拟彩色显示器的颜色）
//4   设置下划线（模拟彩色显示器的颜色）
//5   设置闪烁
//7   设置反向图象
//22  设置一般密度
//24  关闭下划线
//25  关闭闪烁
//27  关闭反向图象
//30  设置黑色前景
//31  设置红色前景
//32  设置绿色前景
//33  设置棕色前景
//34  设置蓝色前景
//35  设置紫色前景
//36  设置青色前景
//37  设置白色前景
//38  在缺省的前景颜色上设置下划线
//39  在缺省的前景颜色上关闭下划线
//40  设置黑色背景
//41  设置红色背景
//42  设置绿色背景
//43  设置棕色背景
//44  设置蓝色背景
//45  设置紫色背景
//46  设置青色背景
//47  设置白色背景
//49  设置缺省黑色背景

enum ShellFontSpec
{
    SHELL_FONT_SPEC_NULL        = 0x00,
    SHELL_FONT_SPEC_BOLD        = 0x01,
    SHELL_FONT_SPEC_UNDERLINE   = 0x02,
    SHELL_FONT_SPEC_FLASH       = 0x04,
    SHELL_FONT_SPEC_DARK        = 0x08,
};

enum ShellFontColor
{
    SHELL_FONT_COLOR_BLACK      = 0x0100, //30
    SHELL_FONT_COLOR_RED        = 0x0200,
    SHELL_FONT_COLOR_GREEN      = 0x0400,
    SHELL_FONT_COLOR_YELLOW     = 0x0800,
    SHELL_FONT_COLOR_BLUE       = 0x1000,
    SHELL_FONT_COLOR_PURPLE     = 0x2000,
    SHELL_FONT_COLOR_CYAN       = 0x4000,
    SHELL_FONT_COLOR_WHITE      = 0x8000,
};

enum ShellFontBackgroundColor
{
    SHELL_FONT_BACKGROUND_COLOR_BLACK      = 0x010000, //40
    SHELL_FONT_BACKGROUND_COLOR_RED        = 0x020000,
    SHELL_FONT_BACKGROUND_COLOR_GREEN      = 0x040000,
    SHELL_FONT_BACKGROUND_COLOR_YELLOW     = 0x080000,
    SHELL_FONT_BACKGROUND_COLOR_BLUE       = 0x100000,
    SHELL_FONT_BACKGROUND_COLOR_PURPLE     = 0x200000,
    SHELL_FONT_BACKGROUND_COLOR_CYAN       = 0x400000,
    SHELL_FONT_BACKGROUND_COLOR_WHITE      = 0x800000,
};

class shell_font
{
private:
    int m_iFlag;
public:
    /**
     * 字体信息
     * @param iFlag
     */
    shell_font(int iFlag = 0);
    virtual ~shell_font();

    /**
     * 生成带样式的文本
     * @param [in] strInput 原始文本
     * @return 生成带样式的文本
     */
    std::string GenerateString(const std::string& strInput);

    /**
     * 生成带样式的文本
     * @param [in] strInput 原始文本
     * @param [in] iFlag 样式
     * @return 生成带样式的文本
     */
    static std::string GenerateString(const std::string& strInput, int iFlag);

    /**
     * 获取样式的生成命令
     * @param [in] iFlag 样式
     * @return 样式的生成命令
     */
    static std::string GetStyleCode(int iFlag);

    /**
     * 获取样式的生成命令
     * @return 样式的生成命令
     */
    std::string GetStyleCode();

    /**
     * 获取样式的关闭命令
     * @return 样式的关闭命令
     */
    static std::string GetStyleCloseCode();
public:
};

#endif /* SHELLFONT_H_ */
