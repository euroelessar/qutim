/****************************************************************************
 *  metainfofields_p.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/


#ifndef METAINFOFIELDS_H
#define METAINFOFIELDS_H

#include <metainfo.h>

namespace qutim_sdk_0_3 {

namespace oscar {

static void init_countries_list(FieldNamesList &list)
{
	list.insert(9999, QT_TRANSLATE_NOOP("Country", "Other"));
	list.insert(93, QT_TRANSLATE_NOOP("Country", "Afghanistan"));
	list.insert(355, QT_TRANSLATE_NOOP("Country", "Albania"));
	list.insert(213, QT_TRANSLATE_NOOP("Country", "Algeria"));
	list.insert(376, QT_TRANSLATE_NOOP("Country", "Andorra"));
	list.insert(244, QT_TRANSLATE_NOOP("Country", "Angola"));
	list.insert(101, QT_TRANSLATE_NOOP("Country", "Anguilla"));
	list.insert(1021, QT_TRANSLATE_NOOP("Country", "Antigua and Barbuda"));
	list.insert(5902, QT_TRANSLATE_NOOP("Country", "Antilles"));
	list.insert(54, QT_TRANSLATE_NOOP("Country", "Argentina"));
	list.insert(374, QT_TRANSLATE_NOOP("Country", "Armenia"));
	list.insert(297, QT_TRANSLATE_NOOP("Country", "Aruba"));
	list.insert(247, QT_TRANSLATE_NOOP("Country", "Ascension Island"));
	list.insert(61, QT_TRANSLATE_NOOP("Country", "Australia"));
	list.insert(43, QT_TRANSLATE_NOOP("Country", "Austria"));
	list.insert(994, QT_TRANSLATE_NOOP("Country", "Azerbaijan"));
	list.insert(103, QT_TRANSLATE_NOOP("Country", "Bahamas"));
	list.insert(973, QT_TRANSLATE_NOOP("Country", "Bahrain"));
	list.insert(880, QT_TRANSLATE_NOOP("Country", "Bangladesh"));
	list.insert(104, QT_TRANSLATE_NOOP("Country", "Barbados"));
	list.insert(120, QT_TRANSLATE_NOOP("Country", "Barbuda"));
	list.insert(375, QT_TRANSLATE_NOOP("Country", "Belarus"));
	list.insert(32, QT_TRANSLATE_NOOP("Country", "Belgium"));
	list.insert(501, QT_TRANSLATE_NOOP("Country", "Belize"));
	list.insert(229, QT_TRANSLATE_NOOP("Country", "Benin"));
	list.insert(105, QT_TRANSLATE_NOOP("Country", "Bermuda"));
	list.insert(975, QT_TRANSLATE_NOOP("Country", "Bhutan"));
	list.insert(591, QT_TRANSLATE_NOOP("Country", "Bolivia"));
	list.insert(387, QT_TRANSLATE_NOOP("Country", "Bosnia and Herzegovina"));
	list.insert(267, QT_TRANSLATE_NOOP("Country", "Botswana"));
	list.insert(55, QT_TRANSLATE_NOOP("Country", "Brazil"));
	list.insert(106, QT_TRANSLATE_NOOP("Country", "British Virgin Islands"));
	list.insert(673, QT_TRANSLATE_NOOP("Country", "Brunei"));
	list.insert(359, QT_TRANSLATE_NOOP("Country", "Bulgaria"));
	list.insert(226, QT_TRANSLATE_NOOP("Country", "Burkina Faso"));
	list.insert(257, QT_TRANSLATE_NOOP("Country", "Burundi"));
	list.insert(855, QT_TRANSLATE_NOOP("Country", "Cambodia"));
	list.insert(237, QT_TRANSLATE_NOOP("Country", "Cameroon"));
	list.insert(107, QT_TRANSLATE_NOOP("Country", "Canada"));
	list.insert(178, QT_TRANSLATE_NOOP("Country", "Canary Islands"));
	list.insert(238, QT_TRANSLATE_NOOP("Country", "Cape Verde Islands"));
	list.insert(108, QT_TRANSLATE_NOOP("Country", "Cayman Islands"));
	list.insert(236, QT_TRANSLATE_NOOP("Country", "Central African Republic"));
	list.insert(235, QT_TRANSLATE_NOOP("Country", "Chad"));
	list.insert(56, QT_TRANSLATE_NOOP("Country", "Chile, Republic of"));
	list.insert(86, QT_TRANSLATE_NOOP("Country", "China"));
	list.insert(672, QT_TRANSLATE_NOOP("Country", "Christmas Island"));
	list.insert(6102, QT_TRANSLATE_NOOP("Country", "Cocos (Keeling) Islands"));
	list.insert(57, QT_TRANSLATE_NOOP("Country", "Colombia"));
	list.insert(2691, QT_TRANSLATE_NOOP("Country", "Comoros"));
	list.insert(242, QT_TRANSLATE_NOOP("Country", "Congo, (Republic of the)"));
	list.insert(243, QT_TRANSLATE_NOOP("Country", "Congo, Democratic Republic of (Zaire)"));
	list.insert(682, QT_TRANSLATE_NOOP("Country", "Cook Islands"));
	list.insert(506, QT_TRANSLATE_NOOP("Country", "Costa Rica"));
	list.insert(225, QT_TRANSLATE_NOOP("Country", "Cote d'Ivoire (Ivory Coast)"));
	list.insert(385, QT_TRANSLATE_NOOP("Country", "Croatia"));
	list.insert(53, QT_TRANSLATE_NOOP("Country", "Cuba"));
	list.insert(357, QT_TRANSLATE_NOOP("Country", "Cyprus"));
	list.insert(42, QT_TRANSLATE_NOOP("Country", "Czech Republic"));
	list.insert(45, QT_TRANSLATE_NOOP("Country", "Denmark"));
	list.insert(246, QT_TRANSLATE_NOOP("Country", "Diego Garcia"));
	list.insert(253, QT_TRANSLATE_NOOP("Country", "Djibouti"));
	list.insert(109, QT_TRANSLATE_NOOP("Country", "Dominica"));
	list.insert(110, QT_TRANSLATE_NOOP("Country", "Dominican Republic"));
	list.insert(593, QT_TRANSLATE_NOOP("Country", "Ecuador"));
	list.insert(20, QT_TRANSLATE_NOOP("Country", "Egypt"));
	list.insert(503, QT_TRANSLATE_NOOP("Country", "El Salvador"));
	list.insert(240, QT_TRANSLATE_NOOP("Country", "Equatorial Guinea"));
	list.insert(291, QT_TRANSLATE_NOOP("Country", "Eritrea"));
	list.insert(372, QT_TRANSLATE_NOOP("Country", "Estonia"));
	list.insert(251, QT_TRANSLATE_NOOP("Country", "Ethiopia"));
	list.insert(298, QT_TRANSLATE_NOOP("Country", "Faeroe Islands"));
	list.insert(500, QT_TRANSLATE_NOOP("Country", "Falkland Islands"));
	list.insert(679, QT_TRANSLATE_NOOP("Country", "Fiji"));
	list.insert(358, QT_TRANSLATE_NOOP("Country", "Finland"));
	list.insert(33, QT_TRANSLATE_NOOP("Country", "France"));
	list.insert(5901, QT_TRANSLATE_NOOP("Country", "French Antilles"));
	list.insert(594, QT_TRANSLATE_NOOP("Country", "French Guiana"));
	list.insert(689, QT_TRANSLATE_NOOP("Country", "French Polynesia"));
	list.insert(241, QT_TRANSLATE_NOOP("Country", "Gabon"));
	list.insert(220, QT_TRANSLATE_NOOP("Country", "Gambia"));
	list.insert(995, QT_TRANSLATE_NOOP("Country", "Georgia"));
	list.insert(49, QT_TRANSLATE_NOOP("Country", "Germany"));
	list.insert(233, QT_TRANSLATE_NOOP("Country", "Ghana"));
	list.insert(350, QT_TRANSLATE_NOOP("Country", "Gibraltar"));
	list.insert(30, QT_TRANSLATE_NOOP("Country", "Greece"));
	list.insert(299, QT_TRANSLATE_NOOP("Country", "Greenland"));
	list.insert(111, QT_TRANSLATE_NOOP("Country", "Grenada"));
	list.insert(590, QT_TRANSLATE_NOOP("Country", "Guadeloupe"));
	list.insert(671, QT_TRANSLATE_NOOP("Country", "Guam, US Territory of"));
	list.insert(502, QT_TRANSLATE_NOOP("Country", "Guatemala"));
	list.insert(224, QT_TRANSLATE_NOOP("Country", "Guinea"));
	list.insert(245, QT_TRANSLATE_NOOP("Country", "Guinea-Bissau"));
	list.insert(592, QT_TRANSLATE_NOOP("Country", "Guyana"));
	list.insert(509, QT_TRANSLATE_NOOP("Country", "Haiti"));
	list.insert(504, QT_TRANSLATE_NOOP("Country", "Honduras"));
	list.insert(852, QT_TRANSLATE_NOOP("Country", "Hong Kong"));
	list.insert(36, QT_TRANSLATE_NOOP("Country", "Hungary"));
	list.insert(354, QT_TRANSLATE_NOOP("Country", "Iceland"));
	list.insert(91, QT_TRANSLATE_NOOP("Country", "India"));
	list.insert(62, QT_TRANSLATE_NOOP("Country", "Indonesia"));
	list.insert(98, QT_TRANSLATE_NOOP("Country", "Iran (Islamic Republic of)"));
	list.insert(964, QT_TRANSLATE_NOOP("Country", "Iraq"));
	list.insert(353, QT_TRANSLATE_NOOP("Country", "Ireland"));
	list.insert(972, QT_TRANSLATE_NOOP("Country", "Israel"));
	list.insert(39, QT_TRANSLATE_NOOP("Country", "Italy"));
	list.insert(112, QT_TRANSLATE_NOOP("Country", "Jamaica"));
	list.insert(81, QT_TRANSLATE_NOOP("Country", "Japan"));
	list.insert(962, QT_TRANSLATE_NOOP("Country", "Jordan"));
	list.insert(705, QT_TRANSLATE_NOOP("Country", "Kazakhstan"));
	list.insert(254, QT_TRANSLATE_NOOP("Country", "Kenya"));
	list.insert(686, QT_TRANSLATE_NOOP("Country", "Kiribati"));
	list.insert(850, QT_TRANSLATE_NOOP("Country", "Korea (North Korea), Democratic People's Republic of"));
	list.insert(82, QT_TRANSLATE_NOOP("Country", "Korea (South Korea), Republic of"));
	list.insert(965, QT_TRANSLATE_NOOP("Country", "Kuwait"));
	list.insert(706, QT_TRANSLATE_NOOP("Country", "Kyrgyzstan"));
	list.insert(856, QT_TRANSLATE_NOOP("Country", "Lao People's Democratic Republic"));
	list.insert(371, QT_TRANSLATE_NOOP("Country", "Latvia"));
	list.insert(961, QT_TRANSLATE_NOOP("Country", "Lebanon"));
	list.insert(266, QT_TRANSLATE_NOOP("Country", "Lesotho"));
	list.insert(231, QT_TRANSLATE_NOOP("Country", "Liberia"));
	list.insert(218, QT_TRANSLATE_NOOP("Country", "Libyan Arab Jamahiriya"));
	list.insert(4101, QT_TRANSLATE_NOOP("Country", "Liechtenstein"));
	list.insert(370, QT_TRANSLATE_NOOP("Country", "Lithuania"));
	list.insert(352, QT_TRANSLATE_NOOP("Country", "Luxembourg"));
	list.insert(853, QT_TRANSLATE_NOOP("Country", "Macau"));
	list.insert(389, QT_TRANSLATE_NOOP("Country", "Macedonia (F.Y.R.O.M.)"));
	list.insert(261, QT_TRANSLATE_NOOP("Country", "Madagascar"));
	list.insert(265, QT_TRANSLATE_NOOP("Country", "Malawi"));
	list.insert(60, QT_TRANSLATE_NOOP("Country", "Malaysia"));
	list.insert(960, QT_TRANSLATE_NOOP("Country", "Maldives"));
	list.insert(223, QT_TRANSLATE_NOOP("Country", "Mali"));
	list.insert(356, QT_TRANSLATE_NOOP("Country", "Malta"));
	list.insert(692, QT_TRANSLATE_NOOP("Country", "Marshall Islands"));
	list.insert(596, QT_TRANSLATE_NOOP("Country", "Martinique"));
	list.insert(222, QT_TRANSLATE_NOOP("Country", "Mauritania"));
	list.insert(230, QT_TRANSLATE_NOOP("Country", "Mauritius"));
	list.insert(269, QT_TRANSLATE_NOOP("Country", "Mayotte Island"));
	list.insert(52, QT_TRANSLATE_NOOP("Country", "Mexico"));
	list.insert(691, QT_TRANSLATE_NOOP("Country", "Micronesia, Federated States of"));
	list.insert(373, QT_TRANSLATE_NOOP("Country", "Moldova, Republic of"));
	list.insert(377, QT_TRANSLATE_NOOP("Country", "Monaco"));
	list.insert(976, QT_TRANSLATE_NOOP("Country", "Mongolia"));
	list.insert(113, QT_TRANSLATE_NOOP("Country", "Montserrat"));
	list.insert(212, QT_TRANSLATE_NOOP("Country", "Morocco"));
	list.insert(258, QT_TRANSLATE_NOOP("Country", "Mozambique"));
	list.insert(95, QT_TRANSLATE_NOOP("Country", "Myanmar"));
	list.insert(264, QT_TRANSLATE_NOOP("Country", "Namibia"));
	list.insert(674, QT_TRANSLATE_NOOP("Country", "Nauru"));
	list.insert(977, QT_TRANSLATE_NOOP("Country", "Nepal"));
	list.insert(31, QT_TRANSLATE_NOOP("Country", "Netherlands"));
	list.insert(599, QT_TRANSLATE_NOOP("Country", "Netherlands Antilles"));
	list.insert(114, QT_TRANSLATE_NOOP("Country", "Nevis"));
	list.insert(687, QT_TRANSLATE_NOOP("Country", "New Caledonia"));
	list.insert(64, QT_TRANSLATE_NOOP("Country", "New Zealand"));
	list.insert(505, QT_TRANSLATE_NOOP("Country", "Nicaragua"));
	list.insert(227, QT_TRANSLATE_NOOP("Country", "Niger"));
	list.insert(234, QT_TRANSLATE_NOOP("Country", "Nigeria"));
	list.insert(683, QT_TRANSLATE_NOOP("Country", "Niue"));
	list.insert(6722, QT_TRANSLATE_NOOP("Country", "Norfolk Island"));
	list.insert(47, QT_TRANSLATE_NOOP("Country", "Norway"));
	list.insert(968, QT_TRANSLATE_NOOP("Country", "Oman"));
	list.insert(92, QT_TRANSLATE_NOOP("Country", "Pakistan"));
	list.insert(680, QT_TRANSLATE_NOOP("Country", "Palau"));
	list.insert(507, QT_TRANSLATE_NOOP("Country", "Panama"));
	list.insert(675, QT_TRANSLATE_NOOP("Country", "Papua New Guinea"));
	list.insert(595, QT_TRANSLATE_NOOP("Country", "Paraguay"));
	list.insert(51, QT_TRANSLATE_NOOP("Country", "Peru"));
	list.insert(63, QT_TRANSLATE_NOOP("Country", "Philippines"));
	list.insert(48, QT_TRANSLATE_NOOP("Country", "Poland"));
	list.insert(351, QT_TRANSLATE_NOOP("Country", "Portugal"));
	list.insert(121, QT_TRANSLATE_NOOP("Country", "Puerto Rico, Common Wealth of"));
	list.insert(974, QT_TRANSLATE_NOOP("Country", "Qatar"));
	list.insert(262, QT_TRANSLATE_NOOP("Country", "Reunion Island"));
	list.insert(40, QT_TRANSLATE_NOOP("Country", "Romania"));
	list.insert(6701, QT_TRANSLATE_NOOP("Country", "Rota Island"));
	list.insert(7, QT_TRANSLATE_NOOP("Country", "Russia"));
	list.insert(250, QT_TRANSLATE_NOOP("Country", "Rwanda"));
	list.insert(290, QT_TRANSLATE_NOOP("Country", "Saint Helena"));
	list.insert(115, QT_TRANSLATE_NOOP("Country", "Saint Kitts"));
	list.insert(1141, QT_TRANSLATE_NOOP("Country", "Saint Kitts and Nevis"));
	list.insert(122, QT_TRANSLATE_NOOP("Country", "Saint Lucia"));
	list.insert(508, QT_TRANSLATE_NOOP("Country", "Saint Pierre and Miquelon"));
	list.insert(116, QT_TRANSLATE_NOOP("Country", "Saint Vincent and the Grenadines"));
	list.insert(670, QT_TRANSLATE_NOOP("Country", "Saipan Island"));
	list.insert(684, QT_TRANSLATE_NOOP("Country", "Samoa"));
	list.insert(378, QT_TRANSLATE_NOOP("Country", "San Marino"));
	list.insert(239, QT_TRANSLATE_NOOP("Country", "Sao Tome & Principe"));
	list.insert(966, QT_TRANSLATE_NOOP("Country", "Saudi Arabia"));
	list.insert(442, QT_TRANSLATE_NOOP("Country", "Scotland"));
	list.insert(221, QT_TRANSLATE_NOOP("Country", "Senegal"));
	list.insert(248, QT_TRANSLATE_NOOP("Country", "Seychelles"));
	list.insert(232, QT_TRANSLATE_NOOP("Country", "Sierra Leone"));
	list.insert(65, QT_TRANSLATE_NOOP("Country", "Singapore"));
	list.insert(4201, QT_TRANSLATE_NOOP("Country", "Slovakia"));
	list.insert(386, QT_TRANSLATE_NOOP("Country", "Slovenia"));
	list.insert(677, QT_TRANSLATE_NOOP("Country", "Solomon Islands"));
	list.insert(252, QT_TRANSLATE_NOOP("Country", "Somalia"));
	list.insert(27, QT_TRANSLATE_NOOP("Country", "South Africa"));
	list.insert(34, QT_TRANSLATE_NOOP("Country", "Spain"));
	list.insert(94, QT_TRANSLATE_NOOP("Country", "Sri Lanka"));
	list.insert(249, QT_TRANSLATE_NOOP("Country", "Sudan"));
	list.insert(597, QT_TRANSLATE_NOOP("Country", "Suriname"));
	list.insert(268, QT_TRANSLATE_NOOP("Country", "Swaziland"));
	list.insert(46, QT_TRANSLATE_NOOP("Country", "Sweden"));
	list.insert(41, QT_TRANSLATE_NOOP("Country", "Switzerland"));
	list.insert(963, QT_TRANSLATE_NOOP("Country", "Syrian Arab Republic"));
	list.insert(886, QT_TRANSLATE_NOOP("Country", "Taiwan"));
	list.insert(708, QT_TRANSLATE_NOOP("Country", "Tajikistan"));
	list.insert(255, QT_TRANSLATE_NOOP("Country", "Tanzania, United Republic of"));
	list.insert(66, QT_TRANSLATE_NOOP("Country", "Thailand"));
	list.insert(6702, QT_TRANSLATE_NOOP("Country", "Tinian Island"));
	list.insert(228, QT_TRANSLATE_NOOP("Country", "Togo"));
	list.insert(690, QT_TRANSLATE_NOOP("Country", "Tokelau"));
	list.insert(676, QT_TRANSLATE_NOOP("Country", "Tonga"));
	list.insert(117, QT_TRANSLATE_NOOP("Country", "Trinidad and Tobago"));
	list.insert(216, QT_TRANSLATE_NOOP("Country", "Tunisia"));
	list.insert(90, QT_TRANSLATE_NOOP("Country", "Turkey"));
	list.insert(709, QT_TRANSLATE_NOOP("Country", "Turkmenistan"));
	list.insert(118, QT_TRANSLATE_NOOP("Country", "Turks and Caicos Islands"));
	list.insert(688, QT_TRANSLATE_NOOP("Country", "Tuvalu"));
	list.insert(256, QT_TRANSLATE_NOOP("Country", "Uganda"));
	list.insert(380, QT_TRANSLATE_NOOP("Country", "Ukraine"));
	list.insert(971, QT_TRANSLATE_NOOP("Country", "United Arab Emirates"));
	list.insert(44, QT_TRANSLATE_NOOP("Country", "United Kingdom"));
	list.insert(598, QT_TRANSLATE_NOOP("Country", "Uruguay"));
	list.insert(1, QT_TRANSLATE_NOOP("Country", "USA"));
	list.insert(711, QT_TRANSLATE_NOOP("Country", "Uzbekistan"));
	list.insert(678, QT_TRANSLATE_NOOP("Country", "Vanuatu"));
	list.insert(379, QT_TRANSLATE_NOOP("Country", "Vatican City"));
	list.insert(58, QT_TRANSLATE_NOOP("Country", "Venezuela"));
	list.insert(84, QT_TRANSLATE_NOOP("Country", "Viet Nam"));
	list.insert(123, QT_TRANSLATE_NOOP("Country", "Virgin Islands of the United States"));
	list.insert(441, QT_TRANSLATE_NOOP("Country", "Wales"));
	list.insert(681, QT_TRANSLATE_NOOP("Country", "Wallis and Futuna Islands"));
	list.insert(685, QT_TRANSLATE_NOOP("Country", "Western Samoa"));
	list.insert(967, QT_TRANSLATE_NOOP("Country", "Yemen"));
	list.insert(381, QT_TRANSLATE_NOOP("Country", "Yugoslavia"));
	list.insert(382, QT_TRANSLATE_NOOP("Country", "Yugoslavia - Montenegro"));
	list.insert(3811, QT_TRANSLATE_NOOP("Country", "Yugoslavia - Serbia"));
	list.insert(260, QT_TRANSLATE_NOOP("Country", "Zambia"));
	list.insert(263, QT_TRANSLATE_NOOP("Country", "Zimbabwe"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, countries, init_countries_list(*x));

static void init_interests_list(FieldNamesList &list)
{
	list.insert(137, QT_TRANSLATE_NOOP("Interest", "50's"));
	list.insert(134, QT_TRANSLATE_NOOP("Interest", "60's"));
	list.insert(135, QT_TRANSLATE_NOOP("Interest", "70's"));
	list.insert(136, QT_TRANSLATE_NOOP("Interest", "80's"));
	list.insert(100, QT_TRANSLATE_NOOP("Interest", "Art"));
	list.insert(128, QT_TRANSLATE_NOOP("Interest", "Astronomy"));
	list.insert(147, QT_TRANSLATE_NOOP("Interest", "Audio and Visual"));
	list.insert(125, QT_TRANSLATE_NOOP("Interest", "Business"));
	list.insert(146, QT_TRANSLATE_NOOP("Interest", "Business Services"));
	list.insert(101, QT_TRANSLATE_NOOP("Interest", "Cars"));
	list.insert(102, QT_TRANSLATE_NOOP("Interest", "Celebrity Fans"));
	list.insert(130, QT_TRANSLATE_NOOP("Interest", "Clothing"));
	list.insert(103, QT_TRANSLATE_NOOP("Interest", "Collections"));
	list.insert(104, QT_TRANSLATE_NOOP("Interest", "Computers"));
	list.insert(105, QT_TRANSLATE_NOOP("Interest", "Culture"));
	list.insert(122, QT_TRANSLATE_NOOP("Interest", "Ecology"));
	list.insert(139, QT_TRANSLATE_NOOP("Interest", "Entertainment"));
	list.insert(138, QT_TRANSLATE_NOOP("Interest", "Finance and Corporate"));
	list.insert(106, QT_TRANSLATE_NOOP("Interest", "Fitness"));
	list.insert(142, QT_TRANSLATE_NOOP("Interest", "Health and Beauty"));
	list.insert(108, QT_TRANSLATE_NOOP("Interest", "Hobbies"));
	list.insert(150, QT_TRANSLATE_NOOP("Interest", "Home Automation"));
	list.insert(144, QT_TRANSLATE_NOOP("Interest", "Household Products"));
	list.insert(107, QT_TRANSLATE_NOOP("Interest", "Games"));
	list.insert(124, QT_TRANSLATE_NOOP("Interest", "Government"));
	list.insert(109, QT_TRANSLATE_NOOP("Interest", "ICQ - Help"));
	list.insert(110, QT_TRANSLATE_NOOP("Interest", "Internet"));
	list.insert(111, QT_TRANSLATE_NOOP("Interest", "Lifestyle"));
	list.insert(145, QT_TRANSLATE_NOOP("Interest", "Mail Order Catalog"));
	list.insert(143, QT_TRANSLATE_NOOP("Interest", "Media"));
	list.insert(112, QT_TRANSLATE_NOOP("Interest", "Movies and TV"));
	list.insert(113, QT_TRANSLATE_NOOP("Interest", "Music"));
	list.insert(126, QT_TRANSLATE_NOOP("Interest", "Mystics"));
	list.insert(123, QT_TRANSLATE_NOOP("Interest", "News and Media"));
	list.insert(114, QT_TRANSLATE_NOOP("Interest", "Outdoors"));
	list.insert(115, QT_TRANSLATE_NOOP("Interest", "Parenting"));
	list.insert(131, QT_TRANSLATE_NOOP("Interest", "Parties"));
	list.insert(116, QT_TRANSLATE_NOOP("Interest", "Pets and Animals"));
	list.insert(149, QT_TRANSLATE_NOOP("Interest", "Publishing"));
	list.insert(117, QT_TRANSLATE_NOOP("Interest", "Religion"));
	list.insert(141, QT_TRANSLATE_NOOP("Interest", "Retail Stores"));
	list.insert(118, QT_TRANSLATE_NOOP("Interest", "Science"));
	list.insert(119, QT_TRANSLATE_NOOP("Interest", "Skills"));
	list.insert(133, QT_TRANSLATE_NOOP("Interest", "Social science"));
	list.insert(129, QT_TRANSLATE_NOOP("Interest", "Space"));
	list.insert(148, QT_TRANSLATE_NOOP("Interest", "Sporting and Athletic"));
	list.insert(120, QT_TRANSLATE_NOOP("Interest", "Sports"));
	list.insert(127, QT_TRANSLATE_NOOP("Interest", "Travel"));
	list.insert(121, QT_TRANSLATE_NOOP("Interest", "Web Design"));
	list.insert(132, QT_TRANSLATE_NOOP("Interest", "Women"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, interests, init_interests_list(*x));

static void init_languages_list(FieldNamesList &list)
{
	list.insert(55, QT_TRANSLATE_NOOP("Language", "Afrikaans"));
	list.insert(58, QT_TRANSLATE_NOOP("Language", "Albanian"));
	list.insert(1, QT_TRANSLATE_NOOP("Language", "Arabic"));
	list.insert(59, QT_TRANSLATE_NOOP("Language", "Armenian"));
	list.insert(68, QT_TRANSLATE_NOOP("Language", "Azerbaijani"));
	list.insert(72, QT_TRANSLATE_NOOP("Language", "Belorussian"));
	list.insert(2, QT_TRANSLATE_NOOP("Language", "Bhojpuri"));
	list.insert(56, QT_TRANSLATE_NOOP("Language", "Bosnian"));
	list.insert(3, QT_TRANSLATE_NOOP("Language", "Bulgarian"));
	list.insert(4, QT_TRANSLATE_NOOP("Language", "Burmese"));
	list.insert(5, QT_TRANSLATE_NOOP("Language", "Cantonese"));
	list.insert(6, QT_TRANSLATE_NOOP("Language", "Catalan"));
	list.insert(61, QT_TRANSLATE_NOOP("Language", "Chamorro"));
	list.insert(7, QT_TRANSLATE_NOOP("Language", "Chinese"));
	list.insert(8, QT_TRANSLATE_NOOP("Language", "Croatian"));
	list.insert(9, QT_TRANSLATE_NOOP("Language", "Czech"));
	list.insert(10, QT_TRANSLATE_NOOP("Language", "Danish"));
	list.insert(11, QT_TRANSLATE_NOOP("Language", "Dutch"));
	list.insert(12, QT_TRANSLATE_NOOP("Language", "English"));
	list.insert(13, QT_TRANSLATE_NOOP("Language", "Esperanto"));
	list.insert(14, QT_TRANSLATE_NOOP("Language", "Estonian"));
	list.insert(15, QT_TRANSLATE_NOOP("Language", "Farsi"));
	list.insert(16, QT_TRANSLATE_NOOP("Language", "Finnish"));
	list.insert(17, QT_TRANSLATE_NOOP("Language", "French"));
	list.insert(18, QT_TRANSLATE_NOOP("Language", "Gaelic"));
	list.insert(19, QT_TRANSLATE_NOOP("Language", "German"));
	list.insert(20, QT_TRANSLATE_NOOP("Language", "Greek"));
	list.insert(70, QT_TRANSLATE_NOOP("Language", "Gujarati"));
	list.insert(21, QT_TRANSLATE_NOOP("Language", "Hebrew"));
	list.insert(22, QT_TRANSLATE_NOOP("Language", "Hindi"));
	list.insert(23, QT_TRANSLATE_NOOP("Language", "Hungarian"));
	list.insert(24, QT_TRANSLATE_NOOP("Language", "Icelandic"));
	list.insert(25, QT_TRANSLATE_NOOP("Language", "Indonesian"));
	list.insert(26, QT_TRANSLATE_NOOP("Language", "Italian"));
	list.insert(27, QT_TRANSLATE_NOOP("Language", "Japanese"));
	list.insert(28, QT_TRANSLATE_NOOP("Language", "Khmer"));
	list.insert(29, QT_TRANSLATE_NOOP("Language", "Korean"));
	list.insert(69, QT_TRANSLATE_NOOP("Language", "Kurdish"));
	list.insert(30, QT_TRANSLATE_NOOP("Language", "Lao"));
	list.insert(31, QT_TRANSLATE_NOOP("Language", "Latvian"));
	list.insert(32, QT_TRANSLATE_NOOP("Language", "Lithuanian"));
	list.insert(65, QT_TRANSLATE_NOOP("Language", "Macedonian"));
	list.insert(33, QT_TRANSLATE_NOOP("Language", "Malay"));
	list.insert(63, QT_TRANSLATE_NOOP("Language", "Mandarin"));
	list.insert(62, QT_TRANSLATE_NOOP("Language", "Mongolian"));
	list.insert(34, QT_TRANSLATE_NOOP("Language", "Norwegian"));
	list.insert(57, QT_TRANSLATE_NOOP("Language", "Persian"));
	list.insert(35, QT_TRANSLATE_NOOP("Language", "Polish"));
	list.insert(36, QT_TRANSLATE_NOOP("Language", "Portuguese"));
	list.insert(60, QT_TRANSLATE_NOOP("Language", "Punjabi"));
	list.insert(37, QT_TRANSLATE_NOOP("Language", "Romanian"));
	list.insert(38, QT_TRANSLATE_NOOP("Language", "Russian"));
	list.insert(39, QT_TRANSLATE_NOOP("Language", "Serbian"));
	list.insert(66, QT_TRANSLATE_NOOP("Language", "Sindhi"));
	list.insert(40, QT_TRANSLATE_NOOP("Language", "Slovak"));
	list.insert(41, QT_TRANSLATE_NOOP("Language", "Slovenian"));
	list.insert(42, QT_TRANSLATE_NOOP("Language", "Somali"));
	list.insert(43, QT_TRANSLATE_NOOP("Language", "Spanish"));
	list.insert(44, QT_TRANSLATE_NOOP("Language", "Swahili"));
	list.insert(45, QT_TRANSLATE_NOOP("Language", "Swedish"));
	list.insert(46, QT_TRANSLATE_NOOP("Language", "Tagalog"));
	list.insert(64, QT_TRANSLATE_NOOP("Language", "Taiwanese"));
	list.insert(71, QT_TRANSLATE_NOOP("Language", "Tamil"));
	list.insert(47, QT_TRANSLATE_NOOP("Language", "Tatar"));
	list.insert(48, QT_TRANSLATE_NOOP("Language", "Thai"));
	list.insert(49, QT_TRANSLATE_NOOP("Language", "Turkish"));
	list.insert(50, QT_TRANSLATE_NOOP("Language", "Ukrainian"));
	list.insert(51, QT_TRANSLATE_NOOP("Language", "Urdu"));
	list.insert(52, QT_TRANSLATE_NOOP("Language", "Vietnamese"));
	list.insert(67, QT_TRANSLATE_NOOP("Language", "Welsh"));
	list.insert(53, QT_TRANSLATE_NOOP("Language", "Yiddish"));
	list.insert(54, QT_TRANSLATE_NOOP("Language", "Yoruba"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, languages, init_languages_list(*x));

static void init_pasts_list(FieldNamesList &list)
{
	list.insert(300, QT_TRANSLATE_NOOP("Past", "Elementary School"));
	list.insert(301, QT_TRANSLATE_NOOP("Past", "High School"));
	list.insert(302, QT_TRANSLATE_NOOP("Past", "College"));
	list.insert(303, QT_TRANSLATE_NOOP("Past", "University"));
	list.insert(304, QT_TRANSLATE_NOOP("Past", "Military"));
	list.insert(305, QT_TRANSLATE_NOOP("Past", "Past Work Place"));
	list.insert(306, QT_TRANSLATE_NOOP("Past", "Past Organization"));
	list.insert(399, QT_TRANSLATE_NOOP("Past", "Other"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, pasts, init_pasts_list(*x));

static void init_genders_list(FieldNamesList &list)
{
	LocalizedString m = QT_TRANSLATE_NOOP("Gender", "Male");
	LocalizedString f = QT_TRANSLATE_NOOP("Gender", "Female");
	list.insert(1, f);
	list.insert(2, m);
	list.insert('F', f);
	list.insert('M', m);
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, genders, init_genders_list(*x));

static void init_study_levels_list(FieldNamesList &list)
{
	list.insert(4, QT_TRANSLATE_NOOP("StudyLevel", "Associated degree"));
	list.insert(5, QT_TRANSLATE_NOOP("StudyLevel", "Bachelor's degree"));
	list.insert(1, QT_TRANSLATE_NOOP("StudyLevel", "Elementary"));
	list.insert(2, QT_TRANSLATE_NOOP("StudyLevel", "High-school"));
	list.insert(6, QT_TRANSLATE_NOOP("StudyLevel", "Master's degree"));
	list.insert(7, QT_TRANSLATE_NOOP("StudyLevel", "PhD"));
	list.insert(8, QT_TRANSLATE_NOOP("StudyLevel", "Postdoctoral"));
	list.insert(3, QT_TRANSLATE_NOOP("StudyLevel", "University / College"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, studyLevels, init_study_levels_list(*x));

static void init_industries_list(FieldNamesList &list)
{
	list.insert(2, QT_TRANSLATE_NOOP("Industry", "Agriculture"));
	list.insert(3, QT_TRANSLATE_NOOP("Industry", "Arts"));
	list.insert(4, QT_TRANSLATE_NOOP("Industry", "Construction"));
	list.insert(5, QT_TRANSLATE_NOOP("Industry", "Consumer Goods"));
	list.insert(6, QT_TRANSLATE_NOOP("Industry", "Corporate Services"));
	list.insert(7, QT_TRANSLATE_NOOP("Industry", "Education"));
	list.insert(8, QT_TRANSLATE_NOOP("Industry", "Finance"));
	list.insert(9, QT_TRANSLATE_NOOP("Industry", "Government"));
	list.insert(10, QT_TRANSLATE_NOOP("Industry", "High Tech"));
	list.insert(11, QT_TRANSLATE_NOOP("Industry", "Legal"));
	list.insert(12, QT_TRANSLATE_NOOP("Industry", "Manufacturing"));
	list.insert(13, QT_TRANSLATE_NOOP("Industry", "Media"));
	list.insert(14, QT_TRANSLATE_NOOP("Industry", "Medical & Health Care"));
	list.insert(15, QT_TRANSLATE_NOOP("Industry", "Non-Profit Organization Management"));
	list.insert(19, QT_TRANSLATE_NOOP("Industry", "Other"));
	list.insert(16, QT_TRANSLATE_NOOP("Industry", "Recreation, Travel & Entertainment"));
	list.insert(17, QT_TRANSLATE_NOOP("Industry", "Service Industry"));
	list.insert(18, QT_TRANSLATE_NOOP("Industry", "Transportation"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, industries, init_industries_list(*x));

static void init_occupations_list(FieldNamesList &list)
{
	list.insert(1, QT_TRANSLATE_NOOP("Occupation", "Academic"));
	list.insert(2, QT_TRANSLATE_NOOP("Occupation", "Administrative"));
	list.insert(3, QT_TRANSLATE_NOOP("Occupation", "Art/Entertainment"));
	list.insert(4, QT_TRANSLATE_NOOP("Occupation", "College Student"));
	list.insert(5, QT_TRANSLATE_NOOP("Occupation", "Computers"));
	list.insert(6, QT_TRANSLATE_NOOP("Occupation", "Community & Social"));
	list.insert(7, QT_TRANSLATE_NOOP("Occupation", "Education"));
	list.insert(8, QT_TRANSLATE_NOOP("Occupation", "Engineering"));
	list.insert(9, QT_TRANSLATE_NOOP("Occupation", "Financial Services"));
	list.insert(10, QT_TRANSLATE_NOOP("Occupation", "Government"));
	list.insert(11, QT_TRANSLATE_NOOP("Occupation", "High School Student"));
	list.insert(12, QT_TRANSLATE_NOOP("Occupation", "Home"));
	list.insert(13, QT_TRANSLATE_NOOP("Occupation", "ICQ - Providing Help"));
	list.insert(14, QT_TRANSLATE_NOOP("Occupation", "Law"));
	list.insert(15, QT_TRANSLATE_NOOP("Occupation", "Managerial"));
	list.insert(16, QT_TRANSLATE_NOOP("Occupation", "Manufacturing"));
	list.insert(17, QT_TRANSLATE_NOOP("Occupation", "Medical/Health"));
	list.insert(18, QT_TRANSLATE_NOOP("Occupation", "Military"));
	list.insert(19, QT_TRANSLATE_NOOP("Occupation", "Non-Government Organization"));
	list.insert(20, QT_TRANSLATE_NOOP("Occupation", "Professional"));
	list.insert(21, QT_TRANSLATE_NOOP("Occupation", "Retail"));
	list.insert(22, QT_TRANSLATE_NOOP("Occupation", "Retired"));
	list.insert(23, QT_TRANSLATE_NOOP("Occupation", "Science & Research"));
	list.insert(24, QT_TRANSLATE_NOOP("Occupation", "Sports"));
	list.insert(25, QT_TRANSLATE_NOOP("Occupation", "Technical"));
	list.insert(26, QT_TRANSLATE_NOOP("Occupation", "University Student"));
	list.insert(27, QT_TRANSLATE_NOOP("Occupation", "Web Building"));
	list.insert(99, QT_TRANSLATE_NOOP("Occupation", "Other Services"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, occupations, init_occupations_list(*x));

static void init_affilations_list(FieldNamesList &list)
{
	list.insert(200, QT_TRANSLATE_NOOP("Affiliation", "Alumni Org."));
	list.insert(201, QT_TRANSLATE_NOOP("Affiliation", "Charity Org."));
	list.insert(202, QT_TRANSLATE_NOOP("Affiliation", "Club/Social Org."));
	list.insert(203, QT_TRANSLATE_NOOP("Affiliation", "Community Org."));
	list.insert(204, QT_TRANSLATE_NOOP("Affiliation", "Cultural Org."));
	list.insert(205, QT_TRANSLATE_NOOP("Affiliation", "Fan Clubs"));
	list.insert(206, QT_TRANSLATE_NOOP("Affiliation", "Fraternity/Sorority"));
	list.insert(207, QT_TRANSLATE_NOOP("Affiliation", "Hobbyists Org."));
	list.insert(208, QT_TRANSLATE_NOOP("Affiliation", "International Org."));
	list.insert(209, QT_TRANSLATE_NOOP("Affiliation", "Nature and Environment Org."));
	list.insert(210, QT_TRANSLATE_NOOP("Affiliation", "Professional Org."));
	list.insert(211, QT_TRANSLATE_NOOP("Affiliation", "Scientific/Technical Org."));
	list.insert(212, QT_TRANSLATE_NOOP("Affiliation", "Self Improvement Group"));
	list.insert(213, QT_TRANSLATE_NOOP("Affiliation", "Spiritual/Religious Org."));
	list.insert(214, QT_TRANSLATE_NOOP("Affiliation", "Sports Org."));
	list.insert(215, QT_TRANSLATE_NOOP("Affiliation", "Support Org."));
	list.insert(216, QT_TRANSLATE_NOOP("Affiliation", "Trade and Business Org."));
	list.insert(217, QT_TRANSLATE_NOOP("Affiliation", "Union"));
	list.insert(218, QT_TRANSLATE_NOOP("Affiliation", "Volunteer Org."));
	list.insert(299, QT_TRANSLATE_NOOP("Affiliation", "Other"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, affilations, init_affilations_list(*x));

typedef QHash<quint32, QString> AgesList;
static void init_ages_list(AgesList &list)
{
	list.insert(0x0011000D, QT_TRANSLATE_NOOP("Age", "13-17"));
	list.insert(0x00160012, QT_TRANSLATE_NOOP("Age", "18-22"));
	list.insert(0x001D0017, QT_TRANSLATE_NOOP("Age", "23-29"));
	list.insert(0x0027001E, QT_TRANSLATE_NOOP("Age", "30-39"));
	list.insert(0x00310028, QT_TRANSLATE_NOOP("Age", "40-49"));
	list.insert(0x003B0032, QT_TRANSLATE_NOOP("Age", "50-59"));
	list.insert(0x2710003C, QT_TRANSLATE_NOOP("Age", "60-above"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(AgesList, ages, init_ages_list(*x));

static void init_maritals_list(FieldNamesList &list)
{
	list.insert(10, QT_TRANSLATE_NOOP("Marital", "Single"));
	list.insert(11, QT_TRANSLATE_NOOP("Marital", "Close relationships"));
	list.insert(12, QT_TRANSLATE_NOOP("Marital", "Engaged"));
	list.insert(20, QT_TRANSLATE_NOOP("Marital", "Married"));
	list.insert(30, QT_TRANSLATE_NOOP("Marital", "Divorced"));
	list.insert(31, QT_TRANSLATE_NOOP("Marital", "Separated"));
	list.insert(40, QT_TRANSLATE_NOOP("Marital", "Widowed"));
	list.insert(50, QT_TRANSLATE_NOOP("Marital", "Open relationship"));
	list.insert(255, QT_TRANSLATE_NOOP("Marital", "Other"));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(FieldNamesList, maritals, init_maritals_list(*x));

} } // namespace qutim_sdk_0_3::Oscar

#endif // METAINFOFIELDS_H
