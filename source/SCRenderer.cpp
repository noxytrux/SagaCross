#include "SCRenderer.h"

using namespace sc;

SCRenderObj::SCRenderObj(const std::shared_ptr<SCOpenGLRenderable> &renderer)
: _renderer(renderer)
{
    memset( Pos, 0 , 12 );
    memset( Rot, 0 , 12 );

    manager->AddObj( this );

    vDir[ 0 ] = 0;
    vDir[ 1 ] = 1;
    vDir[ 2 ] = 0;
}

SCRenderObj::~SCRenderObj() noexcept
{
    manager->DelObj( this );
}

