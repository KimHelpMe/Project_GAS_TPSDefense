// CSK

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/*
* [네이티브 태그를 사용하는 이유]
* 블루프린트 태그 피커로 선택한 태그는 선택 시점엔 유효하지만, 
* 태그 이름이 바뀌어도 컴파일 에러가 발생하지 않아 추적이 어렵다. 
* 반면 네이티브 C++ 태그는 정의와 사용이 코드로 연결되어 있으므로, 
* 이름을 변경하면 컴파일 에러가 즉시 발생해 리팩토링이 안전하다.
*/


//ㅁㄴㅇㄹ
//asdfjfjfj
// develop

// 깃 허브 테스트
// 내 이름은 김창식!
// 안녕하세요 ㅎㅎ

// Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!Warning!!!

//asdf
//sssssssssssssssssssss
// 
//aaaaaaaaaaa


namespace CSGameplayTags
{
	namespace Abilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
	}

	namespace Events
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
	}
}