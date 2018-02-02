/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMSetting.h
 * @brief
 *******************************************************************************
 */
#pragma once

#include <map>
#include <memory>

#include <QString>

namespace DLM
{
enum class TrafficSense
{
    LeftHand,
    RightHand
};

enum class ConnectionType
{
    Continuation,
    Merge,
    Split
};

enum class LaneType
{
    Normal,
    ExitLane,
    EntryLane,
    Emergency,
    Unknown,
    Parking,
    Shoulder
};

enum class Marking
{
    NoMarking,
    LongDashedLine,
    DoubleSolidLine,
    SolidLineDashedLine,
    SingleSolidLine,
    DashedLineSolidLine,
    ShortDashedLine,
    ShadedAreaMarking,
    DashedBlocks,
    DoubleDashedLine,
    CrossingAlert,
    Curb,
    WallFlat,
    WallTunnel,
    BarrierJersey,
    BarrierSound,
    BarrierCable,
    Guardrail,
    Fence
};

enum class Material
{
    Unknown,
    Metal,
    Concrete,
    Stone,
    Wood,
    Plastic,
    Transparent
};

// Map to get string expression of traffic sign enum.
static const std::map<TrafficSense, QString> scTrafficSenseMap =
{
    {TrafficSense::LeftHand,  "LEFT_HAND"},
    {TrafficSense::RightHand, "RIGHT_HAND"}
};

// Map to get string expression of connection type.
static const std::map<ConnectionType, QString> scConnectionTypeMap =
{
    {ConnectionType::Continuation,  "CONTINUATION"},
    {ConnectionType::Merge,         "MERGE"},
    {ConnectionType::Split,         "SPLIT"}
};

// Map to get string expression of lane type.
static const std::map<LaneType, QString> scLaneTypeMap =
{
    {LaneType::Normal,     "NORMAL"},
    {LaneType::ExitLane,   "EXIT_LANE"},
    {LaneType::EntryLane,  "ENTRY_LANE"},
    {LaneType::Emergency,  "EMERGENCY"},
    {LaneType::Unknown,    "UNKNOWN"},
    {LaneType::Parking,    "PARKING"},
    {LaneType::Shoulder,   "SHOULDER"}
};

// Map to get string expression of marking type.
static const std::map<Marking, QString> scMarkingTypeMap =
{
    {Marking::NoMarking,            "NO_MARKING"},
    {Marking::LongDashedLine,       "LONG_DASHED_LINE"},
    {Marking::DoubleSolidLine,      "DOUBLE_SOLID_LINE"},
    {Marking::SolidLineDashedLine,  "SOLID_LINE_DASHED_LINE"},
    {Marking::SingleSolidLine,      "SINGLE_SOLID_LINE"},
    {Marking::DashedLineSolidLine,  "DASHED_LINE_SOLID_LINE"},
    {Marking::ShortDashedLine,      "SHORT_DASHED_LINE"},
    {Marking::ShadedAreaMarking,    "SHADED_AREA_MARKING"},
    {Marking::DashedBlocks,         "DASHED_BLOCKS"},
    {Marking::DoubleDashedLine,     "DOUBLE_DASHED_LINE"},
    {Marking::CrossingAlert,        "CROSSING_ALERT"},
    {Marking::Curb,                 "CURB"},
    {Marking::WallFlat,             "WALL_FLAT"},
    {Marking::WallTunnel,           "WALL_TUNNEL"},
    {Marking::BarrierJersey,        "BARRIER_JERSEY"},
    {Marking::BarrierSound,         "BARRIER_SOUND"},
    {Marking::BarrierCable,         "BARRIER_CABLE"},
    {Marking::Guardrail,            "GUARD_RAIL"},
    {Marking::Fence,                "FENCE"}
};

// Map to get string expression of road material type.
static const std::map<Material, QString> scMaterialTypeMap =
{
    {Material::Unknown,     "UNKNOWN"},
    {Material::Metal,       "METAL"},
    {Material::Concrete,    "CONCRETE"},
    {Material::Stone,       "STONE"},
    {Material::Wood,        "WOOD"},
    {Material::Plastic,     "PLASTIC"},
    {Material::Transparent, "TRANSPARENT"}
};

// Map to get real speed limit from speed limit enum.
static const std::map<std::uint32_t, std::uint16_t> scSpeedLimitTypeValueMap =
{
    {274505, 5}, {274506, 5}, {278505, 5}, {278506, 5},
    {274510, 10}, {274511, 10}, {278510, 10}, {278511, 10},
    {274515, 15}, {274516, 15}, {278515, 15}, {278516, 15},
    {274520, 20}, {274521, 20}, {278520, 20}, {278521, 20},
    {274525, 25}, {274526, 25}, {278525, 25}, {278526, 25},
    {274530, 30}, {274531, 30}, {278530, 30}, {278531, 30},
    {274535, 35}, {274536, 35}, {278535, 35}, {278536, 35},
    {274540, 40}, {274541, 40}, {278540, 40}, {278541, 40},
    {274545, 45}, {274546, 45}, {278545, 45}, {278546, 45},
    {274550, 50}, {274551, 50}, {278550, 50}, {278551, 50},
    {274555, 55}, {274556, 55}, {278555, 55}, {278556, 55},
    {274560, 60}, {274561, 60}, {278560, 60}, {278561, 60},
    {274565, 65}, {274566, 65}, {278565, 65}, {278566, 65},
    {274570, 70}, {274571, 70}, {278570, 70}, {278571, 70},
    {274575, 75}, {274576, 75}, {278575, 75}, {278576, 75},
    {274580, 80}, {274581, 80}, {278580, 80}, {278581, 80},
    {274585, 85}, {274586, 85}, {278585, 85}, {278586, 85},
    {274590, 90}, {274591, 90}, {278590, 90}, {278591, 90},
    {274595, 95}, {274596, 95}, {278595, 95}, {278596, 95},
    {274600, 100}, {274601, 100}, {278600, 100}, {278601, 100},
    {274610, 110}, {274611, 110}, {278610, 110}, {278611, 110},
    {274620, 120}, {274621, 120}, {278620, 120}, {278621, 120},
    {274630, 130}, {274631, 130}, {278630, 130}, {278631, 130},
    {274640, 140}, {274641, 140}, {278640, 140}, {278641, 140},
    {274650, 150}, {274651, 150}, {278650, 150}, {278651, 150},
    {274660, 160}, {274661, 160}, {278660, 160}, {278661, 160},
    {274670, 170}, {274671, 170}, {278670, 170}, {278671, 170},
    {274680, 180}, {274681, 180}, {278680, 180}, {278681, 180},
    {274690, 190}, {274691, 190}, {278690, 190}, {278691, 190}
};

}

namespace Model
{

class DLMSetting
{
public:
    static const std::shared_ptr<DLMSetting>& Instance();

    void SetSourceVersion(const std::uint32_t& aSourceVersion);
    void SetTargetVersion(const std::uint32_t& aTargetVersion);
    void SetBaselineMapId(const std::uint32_t& aBaselineMapId);
    void SetJournalVersion(const QString& aJournalVersion);
    void SetIsoCountryCode(const QString& aCountryCode);
    void SetTrafficSense(const DLM::TrafficSense& aTrafficSense);
    const std::uint32_t& GetSourceVersion() const;
    const std::uint32_t& GetTargetVersion() const;
    const std::uint32_t& GetBaselineMapId() const;
    const QString& GetJournalVersion() const;
    const QString& GetIsoCountryCode() const;
    const DLM::TrafficSense& GetTrafficSense() const;

private:
    DLMSetting();

private:
    // TODO: will provide UI for user to change these values.
    std::uint32_t mSourceVersion;
    std::uint32_t mTargetVersion;
    std::uint32_t mBaselineMapId;
    QString mJournalVersion;
    QString mIsoCountryCode;
    DLM::TrafficSense mTrafficeSense;
};

}
