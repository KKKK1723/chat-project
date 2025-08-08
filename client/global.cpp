#include "global.h"

std::function<void(QWidget *)> repolish = [](QWidget *w)
{
    w->style()->unpolish(w);
    w->style()->polish(w);
};

std::function<QByteArray(const QByteArray &)> Hash = [](const QByteArray &pwd)
{
    QByteArray result = QCryptographicHash::hash(pwd, QCryptographicHash::Sha256);
    return result;
};

QString gate_url_prefix = "";
