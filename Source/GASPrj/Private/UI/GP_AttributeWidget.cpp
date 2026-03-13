
#include "UI/GP_AttributeWidget.h"

#include "Components/ProgressBar.h"


void UGP_AttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair, UGP_AttributeSet* AttributeSet)
{
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);

	OnAttributeChange(AttributeValue, MaxAttributeValue);
}

void UGP_AttributeWidget::OnAttributeChange(float NewValue, float NewMaxValue)
{
	CPP_ProgressBar->SetPercent(NewValue / NewMaxValue);

	if (NewValue <= 0)
		SetVisibility(ESlateVisibility::Collapsed);
	else
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

bool UGP_AttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}
