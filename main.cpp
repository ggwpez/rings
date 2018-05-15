/*
 * SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QPainter>

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

void parse_args(QStringList const &args, quint32 &p, quint32 &s);
QImage render(quint32 const p);
extern QString const help_text;

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	quint32 p, size;
	try
	{
		parse_args(app.arguments(), p, size);
	}
	catch (std::exception const& e)
	{
		qCritical("Error while parsing arguments: %s\n\n%s", qPrintable(e.what()), qPrintable(help_text));
		return 1;
	}

	QImage img = render(p).scaled(size, size);
	QString path = QString("Z-%1Z.%2x%3.png").arg(p).arg(size).arg(size);
	if (!img.save(path))
		throw std::runtime_error("Could not save image.");
	qInfo() << "Saved" << path;
}

///
/// \brief Parses the 0th and 1st command line args as arguments \p p and \p size of type quint32.
/// \param args The arguments passed to parse.
/// \param p Default value is 1024.
/// \param size Default value is \p p.
/// \throws Iff an error occured.
///
void parse_args(QStringList const &args, quint32 &p, quint32 &size)
{
	bool parse_ok;
	p = 1024;

	if (args.size() > 1)
	{
		p = args[1].toUInt(&parse_ok);
		if (!parse_ok)
			throw std::runtime_error("Could not parse first argument as uint.");
	}

	if (args.size() > 2)
	{
		size = args[2].toUInt(&parse_ok);
		if (!parse_ok)
			throw std::runtime_error("Could not parse second argument as uint.");
	}
	else
		size = p;

	if (size > p)
		throw std::runtime_error("Argument 'size 'must not be greater than argument 'p'.");
	qInfo().nospace() << "Using arguments p=" << p << " and size=" << size;
}

///
/// \brief Renders the residue ring of order \p p into an image of size \p p x \p p.
/// \param The order of the ring.
/// \return	A visualisation of the ring.
///
QImage render(quint32 const p)
{
	QImage img = QImage(p, p, QImage::Format_RGB32);
	img.fill(Qt::black);

	for (int x = 0; x < img.width(); ++x)
		for (int y = 0; y < img.height(); ++y)
		{
			double rest = (x * y) % p;
			img.setPixelColor(x, y, QColor{0, int(255 * (rest / p)), 0});
		}

	return img;
}

QString const help_text = "Rings\n"
						  "Render residue class rings of the form \033[1mZ/pZ\033[0m.\n"
						  "The colors indicate the values in the ring, up to a precision of \033[3m1/256\033[0m.\n"
						  "\n"
						  "Usage:\n"
						  "  rings [p] [s]\n"
						  "   p    Order of the ring, type \033[3muint32\033[0m.\n"
						  "        Default value is \033[3m1024\033[0m.\n"
						  "   s    Size of the output image, type \033[3muint32\033[0m.\n"
						  "        Must be smaller or equal to \033[3mp\033[0m.\n"
						  "        Default value is set to \033[3mp\033[0m.\n"
						  "\n"
						  "Example:\n"
						  "  rings \033[3m2048\033[0m \033[3m1024\033[0m\n"
						  "  Would render ring \033[1mZ/2048Z\033[0m and downscale it to \033[3m1024x1024\033[0m.";
