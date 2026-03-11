
#include "UI/GP_WidgetComponent.h"

#include "AbilitySystem/GP_AbilitySystemComponent.h"
#include "AbilitySystem/GP_AttributeSet.h"
#include "Blueprint/WidgetTree.h"
#include "Character/GP_BaseCharacter.h"
#include "UI/GP_AttributeWidget.h"


void UGP_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemData();

	// 만약 프레임 차이로 인한 어빌리티 시스템이 제대로 설정되지 않았을 경우
	// DELEGATE 통해 생성된 시점을 알려준다.
	if (!IsASCInitialized())
	{
		BaseCharacter->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitialized);
		return;
	}

	InitializeAttributeDelegate();
}

void UGP_WidgetComponent::InitAbilitySystemData()
{
	BaseCharacter = Cast<AGP_BaseCharacter>(GetOwner());
	if (BaseCharacter == nullptr) return;
	
	AttributeSet = Cast<UGP_AttributeSet>(BaseCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UGP_AbilitySystemComponent>(BaseCharacter->GetAbilitySystemComponent());
}

void UGP_WidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributeInitialized)
	{
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		BindToAttributeChanges();
	}
}

bool UGP_WidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UGP_WidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// Check if the Attribute Set has been initialized with the first GE
	// if not, bind to some delegate that will be broadcast when it is initialized.
	AbilitySystemComponent = Cast<UGP_AbilitySystemComponent>(ASC);
	AttributeSet = Cast<UGP_AttributeSet>(AS);

	if (!IsASCInitialized()) return;
	InitializeAttributeDelegate();
}

void UGP_WidgetComponent::BindToAttributeChanges()
{
	for (const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair : AttributeMap)
	{
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair);

		GetUserWidgetObject()->WidgetTree->ForEachWidget([this, &Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	}
}

void UGP_WidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UGP_AttributeWidget* AttributeWidget = Cast<UGP_AttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return;							// GP_AttributeWidget인지 체크 후 맞을 경우
	if (!AttributeWidget->MatchesAttributes(Pair)) return;			// 오직 일치하는 속성에 대해서만 동작

	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // 초기값 세팅

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get());	// 게임 진행 도중 변화 감지
	});
}
