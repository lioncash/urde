#pragma once

#include "CGuiWidget.hpp"

namespace urde {

class CGuiCompoundWidget : public CGuiWidget {
public:
  CGuiCompoundWidget(const CGuiWidgetParms& parms);
  FourCC GetWidgetTypeID() const { return FourCC(-1); }

  void OnVisibleChange();
  void OnActiveChange();
  virtual CGuiWidget* GetWorkerWidget(int id) const;
};

} // namespace urde
