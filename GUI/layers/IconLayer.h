//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//      10-9-2015
//
// Authors:
//      michiel
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_ICONLAYER_H
#define OPENPSTD_ICONLAYER_H

#include "GUI/Viewer2D.h"
namespace OpenPSTD
{
    namespace GUI
    {
        class IconLayer : public Layer
        {
        private:
            std::unique_ptr<QOpenGLShaderProgram> program;
            std::unique_ptr<QOpenGLShaderProgram> programIntern;
            unsigned int LineBuffers;
            unsigned int ColorBuffer;
            unsigned int lines;

            std::vector<QVector2D> GetSpeakers(std::shared_ptr<Model> const &m);

            std::vector<QVector2D> GetReceivers(std::shared_ptr<Model> const &m);

        public:
            IconLayer();

            virtual void InitializeGL(QObject *context,
                                      std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) override;

            virtual void PaintGL(QObject *context,
                                 std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) override;

            virtual void UpdateScene(std::shared_ptr<Model> const &m,
                                     std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f) override;

            virtual MinMaxValue GetMinMax() override;
        };

    }
}
#endif //OPENPSTD_ICONLAYER_H
