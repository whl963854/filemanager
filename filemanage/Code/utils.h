#ifndef UTILS_H
#define UTILS_H

#include<QString>
#include<QList>

class Utils
{
public:

    bool static matchLetters(QString lStr,QString sStr);
    QList<QString> static fuzzyMatch(QList<QString>& strs,QString search);

    static bool In(wchar_t start, wchar_t end, wchar_t code);

    static char Convert(int n);

    // 获取第一个汉字的首字母
    static QString GetFirstLetter(const QString &src);

    // 获取所有汉字的首字母
    static QString GetFirstLetters(const QString &src);

    // 获取一个汉字编码的汉语拼音
    static QString GetPinyin(int code);

    // 获取所有汉字的汉语拼音
    static QString GetPinyins(const QString& text);
private:
    Utils();
};

#endif // UTILS_H
