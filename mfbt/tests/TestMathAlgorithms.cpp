/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/MathAlgorithms.h"

#include <stdint.h>

using mozilla::Clamp;
using mozilla::IsPowerOfTwo;

static void TestClamp() {
  MOZ_RELEASE_ASSERT(Clamp(0, 0, 0) == 0);
  MOZ_RELEASE_ASSERT(Clamp(1, 0, 0) == 0);
  MOZ_RELEASE_ASSERT(Clamp(-1, 0, 0) == 0);

  MOZ_RELEASE_ASSERT(Clamp(0, 1, 1) == 1);
  MOZ_RELEASE_ASSERT(Clamp(0, 1, 2) == 1);

  MOZ_RELEASE_ASSERT(Clamp(0, -1, -1) == -1);
  MOZ_RELEASE_ASSERT(Clamp(0, -2, -1) == -1);

  MOZ_RELEASE_ASSERT(Clamp(0, 1, 3) == 1);
  MOZ_RELEASE_ASSERT(Clamp(1, 1, 3) == 1);
  MOZ_RELEASE_ASSERT(Clamp(2, 1, 3) == 2);
  MOZ_RELEASE_ASSERT(Clamp(3, 1, 3) == 3);
  MOZ_RELEASE_ASSERT(Clamp(4, 1, 3) == 3);
  MOZ_RELEASE_ASSERT(Clamp(5, 1, 3) == 3);

  MOZ_RELEASE_ASSERT(Clamp<uint8_t>(UINT8_MAX, 0, UINT8_MAX) == UINT8_MAX);
  MOZ_RELEASE_ASSERT(Clamp<uint8_t>(0, 0, UINT8_MAX) == 0);

  MOZ_RELEASE_ASSERT(Clamp<int8_t>(INT8_MIN, INT8_MIN, INT8_MAX) == INT8_MIN);
  MOZ_RELEASE_ASSERT(Clamp<int8_t>(INT8_MIN, 0, INT8_MAX) == 0);
  MOZ_RELEASE_ASSERT(Clamp<int8_t>(INT8_MAX, INT8_MIN, INT8_MAX) == INT8_MAX);
  MOZ_RELEASE_ASSERT(Clamp<int8_t>(INT8_MAX, INT8_MIN, 0) == 0);
}

static void TestIsPowerOfTwo() {
  static_assert(!IsPowerOfTwo(0u), "0 isn't a power of two");
  static_assert(IsPowerOfTwo(1u), "1 is a power of two");
  static_assert(IsPowerOfTwo(2u), "2 is a power of two");
  static_assert(!IsPowerOfTwo(3u), "3 isn't a power of two");
  static_assert(IsPowerOfTwo(4u), "4 is a power of two");
  static_assert(!IsPowerOfTwo(5u), "5 isn't a power of two");
  static_assert(!IsPowerOfTwo(6u), "6 isn't a power of two");
  static_assert(!IsPowerOfTwo(7u), "7 isn't a power of two");
  static_assert(IsPowerOfTwo(8u), "8 is a power of two");
  static_assert(!IsPowerOfTwo(9u), "9 isn't a power of two");

  static_assert(!IsPowerOfTwo(uint8_t(UINT8_MAX / 2)),
                "127, 0x7f isn't a power of two");
  static_assert(IsPowerOfTwo(uint8_t(UINT8_MAX / 2 + 1)),
                "128, 0x80 is a power of two");
  static_assert(!IsPowerOfTwo(uint8_t(UINT8_MAX / 2 + 2)),
                "129, 0x81 isn't a power of two");
  static_assert(!IsPowerOfTwo(uint8_t(UINT8_MAX - 1)),
                "254, 0xfe isn't a power of two");
  static_assert(!IsPowerOfTwo(uint8_t(UINT8_MAX)),
                "255, 0xff isn't a power of two");

  static_assert(!IsPowerOfTwo(uint16_t(UINT16_MAX / 2)),
                "0x7fff isn't a power of two");
  static_assert(IsPowerOfTwo(uint16_t(UINT16_MAX / 2 + 1)),
                "0x8000 is a power of two");
  static_assert(!IsPowerOfTwo(uint16_t(UINT16_MAX / 2 + 2)),
                "0x8001 isn't a power of two");
  static_assert(!IsPowerOfTwo(uint16_t(UINT16_MAX - 1)),
                "0xfffe isn't a power of two");
  static_assert(!IsPowerOfTwo(uint16_t(UINT16_MAX)),
                "0xffff isn't a power of two");

  static_assert(!IsPowerOfTwo(uint32_t(UINT32_MAX / 2)),
                "0x7fffffff isn't a power of two");
  static_assert(IsPowerOfTwo(uint32_t(UINT32_MAX / 2 + 1)),
                "0x80000000 is a power of two");
  static_assert(!IsPowerOfTwo(uint32_t(UINT32_MAX / 2 + 2)),
                "0x80000001 isn't a power of two");
  static_assert(!IsPowerOfTwo(uint32_t(UINT32_MAX - 1)),
                "0xfffffffe isn't a power of two");
  static_assert(!IsPowerOfTwo(uint32_t(UINT32_MAX)),
                "0xffffffff isn't a power of two");

  static_assert(!IsPowerOfTwo(uint64_t(UINT64_MAX / 2)),
                "0x7fffffffffffffff isn't a power of two");
  static_assert(IsPowerOfTwo(uint64_t(UINT64_MAX / 2 + 1)),
                "0x8000000000000000 is a power of two");
  static_assert(!IsPowerOfTwo(uint64_t(UINT64_MAX / 2 + 2)),
                "0x8000000000000001 isn't a power of two");
  static_assert(!IsPowerOfTwo(uint64_t(UINT64_MAX - 1)),
                "0xfffffffffffffffe isn't a power of two");
  static_assert(!IsPowerOfTwo(uint64_t(UINT64_MAX)),
                "0xffffffffffffffff isn't a power of two");
}

void TestGCD() {
  MOZ_RELEASE_ASSERT(mozilla::GCD(0, 0) == 0);

  // clang-format off
  // import random
  // import math
  //
  // j = 0
  // testcases = [
  //     { "name": "signed 64-bits integers", "upper": (2**63)-1, "suffix": "" },
  //     { "name": "unsigned 64-bits integers", "upper": (2**64)-1, "suffix": "u" },
  //     { "name": "signed 32-bits integers", "upper": (2**31)-1, "suffix": "" },
  //     { "name": "unsigned 32-bits integers", "upper": (2**32)-1, "suffix": "u" },
  // ]
  // for case in testcases:
  //     print("")
  //     print(f"// {case['name']}")
  //     while True:
  //         a = random.randrange(0, case["upper"])
  //         b = random.randrange(0, a)
  //         res = math.gcd(a, b)
  //         j+=1
  //         suffix = case["suffix"]
  //         print(f'MOZ_RELEASE_ASSERT(mozilla::GCD({a}{suffix}, {b}{suffix}) == {res}{suffix});')
  //         if j == 100:
  //             j = 0
  //             break
  //
  // clang-format on

  // signed 64-bits integers
  MOZ_RELEASE_ASSERT(mozilla::GCD(6855423437784447881, 5744152981668854128) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2560787397587345465, 208780102238346432) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5577889716064657494, 2159469434101077254) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5349904765384950054, 1944688623103480392) ==
                     18);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6510887230309733540, 4404045615056449988) ==
                     4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5153663464686238190, 3495293373406661950) ==
                     10);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8640438456651239176, 6172550763106125918) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4636330475123995525, 2504439215041170117) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3049680828923698889, 1798896016456058960) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6857469018143857254, 839235513850919013) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1667993323500460751, 403448480939209779) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2756773685517793960, 1001994517356200529) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5809484314452898314, 252378426271103138) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5756566734144094840, 3050839541929564330) ==
                     10);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2669472117169059649, 1053394704248223342) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8486335744011214524, 4866724521619209633) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4841597191067437171, 1862876789330567260) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8940692064089049746, 6136664682975600685) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6274111242168941448, 688426762929457484) ==
                     4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7715132980994738435, 1456592620536615117) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5650339953233205545, 4406664870835551648) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7763657864638523008, 306878184260935929) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7776062097319502113, 7551650059636008893) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9158681410218029314, 5401644381866109508) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(428865066965126615, 345306139889243757) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1334408785926182232, 736025095410140597) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9129011607893106326, 4818080883860535758) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5968300398911311896, 2550670869539540947) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5030190181362172874, 3861860193070954804) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5449912203994605772, 395450435226244945) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3510149608312823296, 1122015596295686144) ==
                     512);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8822408923914428398, 3005499570530356734) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1894251920744324374, 29251650223056432) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1643262375132697825, 133049278064101269) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5979771268022611030, 5021008984454830630) ==
                     10);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8551631013482492569, 3214028471848344275) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1374240599294724199, 1106817149419837791) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7877493197090616258, 3627451313613172281) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7323120572203017429, 5958183356236253053) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7356702947943126364, 1234023498733740170) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3533663535984312691, 1287666490057924782) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8249625410612436788, 1692674983510387167) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6590544882911640025, 6518468963976945930) ==
                     5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1161703442901270391, 72640111759506406) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3648054318401558456, 286110734809583843) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7445158880116265073, 4921289272987608741) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8052135113655284875, 6319225376882653323) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1272523803145322419, 669368693174176828) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(762600464449954636, 258101161586809942) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8711570456095175409, 3217102356729157526) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8596472485422071677, 6590296624757765441) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8830210169177656300, 4853400012200083924) ==
                     4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2241405940749418043, 1414859858059940275) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6645372226653882826, 1089866326575332751) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4972052091595687646, 3420503469411720440) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8796611232338780872, 8344997795629414169) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4109837086789844244, 2749395249398063222) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7099065868279436275, 3485530390566515044) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1041731907675308955, 561481363772326233) ==
                     9);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5882271298652803063, 5189002859026699540) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(835073783923421192, 56853706366082462) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2514946180207195049, 1934146334993787393) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8975439209128912747, 1377234541321015082) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7039355952603350033, 6501349986472883135) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3747474677542899887, 2583298074596991574) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8176323250144977780, 4706420973964948943) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8748260715055109420, 7094433080013425893) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2192085035443314042, 1964458338792492837) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4387059045133366080, 1521989527531982075) ==
                     5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5735277355594712161, 1564786041102368131) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3898210686025675418, 1252531932064281967) ==
                     7);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1886253648955280570, 235795900409586307) ==
                     7);
  MOZ_RELEASE_ASSERT(mozilla::GCD(862214669576776425, 90702464427080315) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2831206027654482398, 2543050780384667441) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3561377609788845927, 2837335262531584639) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1973347825404473626, 634138253455209313) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6447708134022060248, 4346890077474767787) ==
                     19);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1690365172062143048, 678324119874104971) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2900650911116509049, 818833306053988358) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4126258648185074937, 2190040072639642009) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7310083765892765377, 3615506256861011852) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1482494462925181129, 568665115985247457) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2675477464881771327, 1476381757716745502) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6437060864565620566, 266707802567839796) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3800292251587454230, 245022706279648741) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3549515343757259493, 1328377263505490456) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8324574140787708570, 393444007055415700) ==
                     10);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4373054321374923750, 1031193918836627100) ==
                     150);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1370218692062991327, 682070501541164452) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4728813669404513421, 2346998232227619529) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(320864023853706984, 50178854177191437) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5424710852893793602, 4237974770221703674) ==
                     2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5167582806125634015, 3538730725111557853) ==
                     79);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7197930858946883500, 6668556859540800605) ==
                     5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2900089593575477549, 2554913303396097824) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1397576820519717048, 847997331257829237) ==
                     3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3939714364354053162, 1374067007308181723) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1065626084531260890, 664198963621954813) ==
                     317);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5912876357514418196, 5112700044139286313) ==
                     1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2654316726913809362, 588030922713986903) ==
                     1);

  // unsigned 64-bits integers
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(16747832015348854198u, 10986175599217457242u) == 2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(14011882763672869646u, 1150181481133900726u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6605029198216299492u, 2540177763690679863u) ==
                     3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8723446333453359635u, 5501999887069319528u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2056609692029140361u, 1456692183174011231u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3979920159703007405u, 2102351633956912159u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9463892761763926474u, 5727651032816755587u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15074653294321365395u, 7500084005319994862u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7596876989397200146u, 2100623677138635163u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15788975435035111366u, 13949507094186899135u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(11511089994271140687u, 11202842908571961185u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5238481506779057035u, 1275096406977139452u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(10319988989820236521u, 6004256112028859859u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15363016657999062582u, 13709656670722381934u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1212882338768103987u, 400304873392680016u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(14516701884936382582u, 9474965125574306885u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4684990176797036518u, 2826010316418750908u) ==
                     2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1257550743165743081u, 501524040422212694u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2684107647237574540u, 1059404913392538915u) ==
                     5u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9075798209725656040u, 8460431147770771484u) ==
                     4u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8849414266308239550u, 2100344973594953676u) ==
                     6u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(18235452615524492166u, 6948238589518088517u) == 3u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15050298436941428700u, 1467533438133155187u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8834598722016252963u, 4311275747815972852u) ==
                     17u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9356558625132137133u, 2037947968328350721u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3849613153563955590u, 742698742609310596u) ==
                     2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(14456988562990139501u, 10112205238651656021u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12307508681986233124u, 9812326358082292497u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1542509761845906606u, 753342053499303952u) ==
                     2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3002452874498902380u, 1551203246991573851u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6995746439795805457u, 1188069610619158471u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2746395460341933223u, 2567350813567392270u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(13780256804547757349u, 3248441336598733689u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(11585262422698980788u, 9223319679416307971u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12061506913736835258u, 4388981418731026638u) == 54u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7926097431519628264u, 6609465824726553267u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4869073093357623730u, 127092341961569309u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2415749375652736599u, 1225333195065764619u) ==
                     3u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12396258519293261927u, 7854932518032305093u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8482841866529133449u, 8041279973223483861u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6256232276718808317u, 218093546248209886u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8708964372422992556u, 5925839455605803265u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7079489553626522083u, 2723660727447617723u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6456428365552053201u, 1199403261032183111u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3346567208089938575u, 2383119761029013459u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(11371634586699820652u, 1314783250642191861u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(17865943339510318926u, 6852058968402585010u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6184068614737379672u, 3615164034002231440u) ==
                     8u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4188759555626894588u, 756597961380253895u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9834711092513827417u, 3337572906055372223u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15971004526745900665u, 8185256010881285296u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(15018742812984668959u, 529070670894924960u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1067863751656464299u, 905318428655384382u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(13862829046112265837u, 6101005940549725663u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5042641015440071021u, 3851032995323622058u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12302889786666538640u, 10776548976024201292u) == 76u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(13722399417473040071u, 9411461429949802122u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7320504128957551347u, 54052915134765261u) ==
                     3u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15757615267691124901u, 6960991167654285257u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5748033181727727936u, 303811493931685833u) ==
                     3u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(13393585076101458038u, 11704741982068090192u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1305962146520003941u, 900947650687182151u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(10210329619324275486u, 9165444096209531122u) == 2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12287397750298100333u, 4589303685754232593u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(13074046732385479094u, 9410427502131685240u) == 2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(10769225306727183116u, 3766083633148275570u) == 2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(16097129444752648454u, 1689032025737433449u) == 1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12569058547490329992u, 3311470626838389990u) == 18u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6800922789750937338u, 1401809431753492506u) ==
                     6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7640775166765881526u, 330467034911649653u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(7713745971481011689u, 2881741428874316968u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3447718804232188171u, 2048968371582835027u) ==
                     17u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5048117340512952935u, 2723523492436699844u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(10307361968692211723u, 428905266774914488u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1319115090575683914u, 1262779939989801116u) ==
                     2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12690110976610715926u, 1527151730024909348u) == 2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(12963032302522784237u, 8894543024067386192u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4719664701853305298u, 328290838903591497u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6046363361224867225u, 2463351775539510194u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(467063656725960574u, 62796777888499328u) ==
                     2u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(16390445286228133923u, 3793827091023779027u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3118497337756941652u, 2860811741849353064u) ==
                     4u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(17480668716240157222u, 6736393718990377613u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3008091962262081749u, 2764474578829797968u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(10443605258088065132u, 1118236736154633837u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(8681282777233478597u, 2520450074320754822u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2428054799146631800u, 2304419668216461210u) ==
                     10u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(11986346113373252908u, 5868466983065345812u) == 4u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(566070446598076689u, 226910043938150340u) ==
                     3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(20286446051392853u, 2253005103754547u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(9478145873341733534u, 1361277916695374175u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2194077616952029858u, 1880982148321238243u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5067528875217388843u, 1007391120419508106u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(14964775244731205772u, 8476706085421248933u) == 3u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(15864657026011160414u, 11542748143033682677u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(6819186727513097073u, 3374817819083626717u) ==
                     1u);
  MOZ_RELEASE_ASSERT(
      mozilla::GCD(14864653919493481829u, 1475678482546800916u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(674964986925038761u, 500070581922501698u) ==
                     1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(5286379749864372936u, 1077542296477907313u) ==
                     3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(506827427986892036u, 177356571976309469u) ==
                     1u);

  // signed 32-bits integers
  MOZ_RELEASE_ASSERT(mozilla::GCD(2082847559, 1075502059) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1516817880, 1427978452) == 4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1912103032, 865754441) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1907998028, 1578360455) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2082786344, 1864664012) == 4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2060961011, 1928455778) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(970664659, 63074065) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(55960901, 36955491) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1136602528, 339758054) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2040420582, 1355439044) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1295522905, 736231412) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(778941225, 674482877) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(291862772, 262751987) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(233275018, 60278627) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(701740307, 432255046) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(582766531, 457298210) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(196369046, 15577226) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1342156837, 2790339) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(502348102, 151073265) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(836867611, 797891653) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(859055751, 525520896) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(701234220, 683730404) == 4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2102253469, 1046820362) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1712691453, 34616585) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1074235876, 683609889) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(535965177, 182306069) == 11);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1437763442, 180698008) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2005641602, 175306737) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(803294953, 565920364) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2135931435, 220153322) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1002010726, 619364124) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1841159587, 577256747) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2117547620, 896973794) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2004836234, 157238204) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(952368407, 625062194) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(671144794, 357719289) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1369585680, 279330845) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(389855496, 230820785) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2101505071, 572728762) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1657802296, 667524476) == 4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1007298072, 598682608) == 8);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1499193816, 44129206) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1355799723, 1163556923) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(346410469, 294136125) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(240297386, 239749630) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1595986655, 706220030) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(265850446, 117414954) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1594478812, 559606261) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1098933117, 145267674) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(37749195, 34174284) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(173141528, 158277345) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1523316779, 1507242666) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1574321272, 213222586) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(186241582, 58675779) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1351024876, 1256961567) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2060871503, 1626844669) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(794617235, 606782933) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(620853401, 550785717) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(978990617, 684228903) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(185414372, 160958435) == 11);
  MOZ_RELEASE_ASSERT(mozilla::GCD(13886275, 10781501) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(316445410, 72994145) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(260685833, 66561321) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(656788852, 619471100) == 4);
  MOZ_RELEASE_ASSERT(mozilla::GCD(409924450, 323144710) == 10);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1696374689, 155122424) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1720449495, 1332196090) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(102504868, 95625294) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(959039064, 266180243) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(771762738, 99126507) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1666721205, 164347293) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1145868726, 1013299840) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(123667035, 6968726) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(856285310, 669026117) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1748843942, 376021862) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1364381942, 1316920424) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(376501104, 233350000) == 16);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1516376773, 554534905) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1355209533, 371401397) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(488029245, 453641230) == 5);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2086782535, 1965901533) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1701843138, 197489892) == 6);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1857287302, 756127018) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1806613582, 963087217) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1350708388, 1013432485) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(742201232, 486590366) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(47378255, 18524009) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(750926792, 282203477) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1242468272, 1225593358) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1937337947, 1233008310) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(600511783, 563234297) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1583895113, 1400349394) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(361950446, 20294144) == 26);
  MOZ_RELEASE_ASSERT(mozilla::GCD(712527923, 351368901) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(221252886, 13768150) == 2);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1217530242, 184772639) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1145522580, 92958612) == 12);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1765854048, 1073605551) == 3);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1179258112, 1148756377) == 1);
  MOZ_RELEASE_ASSERT(mozilla::GCD(211982661, 145365362) == 1);

  // unsigned 32-bits integers
  MOZ_RELEASE_ASSERT(mozilla::GCD(2346624228u, 854636854u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(257647411u, 113262213u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(532130107u, 181815062u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(188329196u, 21767880u) == 4u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(965417460u, 433449910u) == 10u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4285939108u, 782087256u) == 4u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3176833937u, 905249796u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1596497177u, 1259467765u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(296928708u, 137867254u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(810260571u, 278688539u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2319673546u, 6698908u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(335032855u, 304923748u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1520046075u, 30861208u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3370242674u, 2513781509u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2380615411u, 41999289u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2999947090u, 619047913u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(463491935u, 219826435u) == 5u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(256795166u, 3240595u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3794760062u, 542176354u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2347135107u, 532837578u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(215263644u, 82185110u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3242470340u, 1014909501u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1935066897u, 1646318370u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2528019825u, 2199478105u) == 5u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(814340701u, 505422837u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2422005621u, 1270490106u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2196878780u, 2125974315u) == 5u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3243580525u, 3222120645u) == 5u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(592838u, 333273u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(957856834u, 660922287u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2650657380u, 2507896759u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(35861051u, 25878355u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1907977010u, 514369620u) == 10u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1850153182u, 1133466079u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2404132308u, 942620249u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4120768767u, 794728522u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3115077311u, 437206010u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3653354572u, 3501340268u) == 4u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3700775106u, 1237309608u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3838425682u, 2767520531u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(812123689u, 691153768u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3201500844u, 1530832674u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(802121923u, 753535009u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(575392026u, 450096822u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1074039450u, 724299558u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3785968159u, 230568577u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(80611731u, 30537579u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3717744094u, 3192172824u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3481208739u, 3389567399u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1126134290u, 760589919u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2452072599u, 1235840929u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4172574373u, 664346996u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4280275945u, 1940565231u) == 11u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1138803378u, 919205598u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3871971423u, 3071143517u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1889403334u, 261936800u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1233462464u, 462090021u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(267801361u, 177041892u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1586528261u, 1146114428u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2209381020u, 1616518545u) == 15u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2493819993u, 110364986u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(105420984u, 83814372u) == 12u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3093899047u, 917349662u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3716325890u, 1554865432u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(692565714u, 265467690u) == 18u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(659720171u, 250624014u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1890623148u, 1632453222u) == 6u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3557986303u, 752931252u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(237903157u, 177153319u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4133928804u, 3898800943u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1783300920u, 196251347u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2035190407u, 866039372u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3893680107u, 3211053018u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4293646715u, 2698207329u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1409442959u, 151043902u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1823328305u, 375231671u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2574512647u, 1902834298u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2533783127u, 1232079823u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2622446878u, 193328426u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4099571222u, 3439224331u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2355797345u, 430435034u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2654318392u, 2069135952u) == 8u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1671976410u, 1100794671u) == 3u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(328877177u, 236038245u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2373247523u, 1198763899u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3230550971u, 203517406u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2274958703u, 353643804u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1048415366u, 740416576u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2768590397u, 843179468u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2839858158u, 1019946790u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4116867766u, 52672530u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3433787325u, 2398189631u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2636022376u, 2289412838u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(2904900253u, 2748915828u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(4041240379u, 605321815u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1730010566u, 92436785u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1362635513u, 757365378u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(1327133482u, 940350094u) == 2u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(3515019959u, 810874750u) == 1u);
  MOZ_RELEASE_ASSERT(mozilla::GCD(82871503u, 43900000u) == 1u);

  MOZ_RELEASE_ASSERT(mozilla::GCD(3u, 7u) == 1u);
}

int main() {
  TestIsPowerOfTwo();
  TestClamp();
  TestGCD();

  return 0;
}