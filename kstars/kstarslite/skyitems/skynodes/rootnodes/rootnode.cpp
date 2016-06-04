#include <QSGTexture>
#include <QQuickWindow>

#include "rootnode.h"
#include "skymaplite.h"
#include "projections/projector.h"
#include "../skynode.h"

RootNode::RootNode()
    :m_clipNode(new QSGGeometryNode), m_skyMapLite(SkyMapLite::Instance()),
      m_clipGeometry(0)
{
    genCachedTextures();
    updateClipPoly();
   // setIsRectangular(false);
}

void RootNode::genCachedTextures() {
    QVector<QVector<QPixmap*>> images = m_skyMapLite->getImageCache();

    QQuickWindow *win = m_skyMapLite->window();

    m_textureCache = QVector<QVector<QSGTexture*>> (images.length());

    for(int i = 0; i < m_textureCache.length(); ++i) {
        int length = images[i].length();
        m_textureCache[i] = QVector<QSGTexture *>(length);
        for(int c = 1; c < length; ++c) {
            m_textureCache[i][c] = win->createTextureFromImage(images[i][c]->toImage());
        }
    }
}

QSGTexture* RootNode::getCachedTexture(int size, char spType) {
    return m_textureCache[SkyMapLite::Instance()->harvardToIndex(spType)][size];
}

void RootNode::appendSkyNode(SkyNode *skyNode) {
    m_skyNodes.append(skyNode);
    appendChildNode(skyNode);
}

void RootNode::prependSkyNode(SkyNode *skyNode) {
    m_skyNodes.append(skyNode);
    prependChildNode(skyNode);
}

void RootNode::removeSkyNode(SkyNode *skyNode) {
    removeChildNode(skyNode);
    m_skyNodes.removeOne(skyNode);
}

void RootNode::removeAllSkyNodes() {
    for(int i = 0; i < m_skyNodes.length(); ++i) {
        removeChildNode(m_skyNodes[i]);
        delete m_skyNodes[i];
    }
    m_skyNodes.clear();
}

void RootNode::updateClipPoly() {
    /*QPolygonF newClip = m_skyMapLite->projector()->clipPoly();
    if(m_clipPoly != newClip) {
        m_clipPoly = newClip;
    }
    QVector<QPointF> triangles;

    for(int i = 1; i < m_clipPoly.size() - 1; ++i) {
        triangles.append(m_clipPoly[0]);
        triangles.append(m_clipPoly[i]);
        triangles.append(m_clipPoly[i+1]);
    }

    const int size = triangles.size();
    if(!m_clipGeometry) {
        m_clipGeometry = new QSGGeometry (QSGGeometry::defaultAttributes_Point2D (),
                                          size);
        m_clipGeometry->setDrawingMode(GL_TRIANGLES);
        setGeometry(m_clipGeometry);
    } else {
        m_clipGeometry->allocate(size);
    }

    QSGGeometry::Point2D * vertex = m_clipGeometry->vertexDataAsPoint2D ();
    for (int i = 0; i < size; i++) {
        vertex[i].x = triangles[i].x();
        vertex[i].y = triangles[i].y();
    }
    m_clipNode->markDirty(QSGNode::DirtyGeometry);*/
}

