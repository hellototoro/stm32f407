#include <gui/clockscreen_screen/clockScreenView.hpp>
#include <gui/clockscreen_screen/clockScreenPresenter.hpp>

clockScreenPresenter::clockScreenPresenter(clockScreenView& v)
    : view(v)
{

}

void clockScreenPresenter::activate()
{

}

void clockScreenPresenter::deactivate()
{

}

void clockScreenPresenter::updateRTC_Time(void)
{
    view.updateRTC_Time();
}