#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>
CommonUtils::CommonUtils()
{
}

//获取圆形图片，
//和BasicWindow类中的getRoundPixmap函数功能相同，参数是原图片和掩膜图片的大小（默认为0，0，即与原图片大小相同）
QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap &mask, QSize masksize)
{
	if (masksize == QSize(0, 0))
	{
		masksize = mask.size();
	}
	else
	{
		mask = mask.scaled(masksize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(masksize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}
//加载样式表，用于设置窗口的外观和风格
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName)
{
	QFile file(":/Resourses/QSS/"+sheetName+".css");
	if (file.open(QFile::ReadOnly))
	{
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}
	file.close();
}

void CommonUtils::setDefaultSkinColor(const QColor& color)	//设置默认皮肤颜色
{
	//重写配置信息
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	//创建一个QSettings对象，指定配置文件路径和格式，进行读写操作
	QSettings settings(path, QSettings::IniFormat);	
	settings.setValue("DefaultSkin/Red", color.red());		//将颜色的红色分量值写入配置文件，键为"SkinColor/Red"
	settings.setValue("DefaultSkin/Green", color.green());	//将颜色的绿色分量值写入配置文件，键为"SkinColor/Green"
	settings.setValue("DefaultSkin/Blue", color.blue());		//将颜色的蓝色分量值写入配置文件，键为"SkinColor/Blue"
	
}

QColor CommonUtils::getDefaultSkinColor()		//获取默认皮肤颜色
{
	QColor color;
	const QString && path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	if(!QFile::exists(path))
		setDefaultSkinColor(QColor(22, 154, 218));	//如果配置文件不存在，则创建一个默认的配置文件，并将默认皮肤颜色设置

	QSettings settings(path, QSettings::IniFormat);
	color.setRed(settings.value("DefaultSkin/Red").toInt());		//从配置文件中读取颜色的红色分量值，键为"SkinColor/Red"，并将其设置到color对象中
	color.setGreen(settings.value("DefaultSkin/Green").toInt());	//从配置文件中读取颜色的绿色分量值，键为"SkinColor/Green"，并将其设置到color对象中
	color.setBlue(settings.value("DefaultSkin/Blue").toInt());	//从配置文件中读取颜色的蓝色分量值，键为"SkinColor/Blue"，并将其设置到color对象中
	return color;
}