#include "CErrorOutputWindow.hpp"

namespace urde
{

CErrorOutputWindow::CErrorOutputWindow(bool flag)
: CIOWin("Error Output Window")
{
    x18_25_ = true;
    x18_26_ = true;
    x18_27_ = true;
    x18_28_ = flag;
}

CIOWin::EMessageReturn CErrorOutputWindow::OnMessage(const CArchitectureMessage&, CArchitectureQueue&)
{
    return EMessageReturn::Normal;
}

void CErrorOutputWindow::Draw() const
{
}

}
