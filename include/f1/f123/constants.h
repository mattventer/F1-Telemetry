#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace F123
{
    enum EPacketId : uint8_t
    {
        Motion = 0,              // Contains all motion data for player’s car – only sent while player is in control
        Session = 1,             // Data about the session – track, time left
        LapData = 2,             // Data about all the lap times of cars in the session
        Event = 3,               // Various notable events that happen during a session
        Participants = 4,        // List of participants in the session, mostly relevant for multiplayer
        CarSetups = 5,           // Packet detailing car setups for cars in the race
        CarTelemetry = 6,        // Telemetry data for all cars
        CarStatus = 7,           // Status data for all cars
        FinalClassification = 8, // Final classification confirmation at the end of a race
        LobbyInfo = 9,           // Information about players in a multiplayer lobby
        CarDamage = 10,          // Damage status for all cars
        SessionHistory = 11,     // Lap and tyre data for session
        TyreSets = 12,           // Extended tyre set data
        MotionEx = 13,           // Extended motion data for player car
        TimeTrial = 14,          // Time Trial specific data
        LapPositions = 15,       // Lap positions on each lap so a chart can be constructed
        Max
    };

    static const EPacketId sPacketIds[16] = {
        EPacketId::Motion,
        EPacketId::Session,
        EPacketId::LapData,
        EPacketId::Event,
        EPacketId::Participants,
        EPacketId::CarSetups,
        EPacketId::CarTelemetry,
        EPacketId::CarStatus,
        EPacketId::FinalClassification,
        EPacketId::LobbyInfo,
        EPacketId::CarDamage,
        EPacketId::SessionHistory,
        EPacketId::TyreSets,
        EPacketId::MotionEx,
        EPacketId::TimeTrial,
        EPacketId::LapPositions,
    };

    // F1 Modern only
    enum class EActualTyreCompound : uint8_t
    {
        Inter = 7,
        Wet = 8,
        C5 = 16,
        C4 = 17,
        C3 = 18,
        C2 = 19,
        C1 = 20,
        C0 = 21,
        Unknown = 22
    };

    // F1 Modern/Classic only
    enum class EVisualTyreCompound : uint8_t
    {
        Inter = 7,
        Wet = 8,
        Soft = 16,
        Medium = 17,
        Hard = 18,
        Unknown = 23
    };

    static const std::map<EActualTyreCompound, std::string> sActualTyreCompoundToString = {{EActualTyreCompound::Inter, "Inter"}, {EActualTyreCompound::Wet, "Wet"}, {EActualTyreCompound::C5, "C5"}, {EActualTyreCompound::C4, "C4"}, {EActualTyreCompound::C3, "C3"}, {EActualTyreCompound::C2, "C2"}, {EActualTyreCompound::C1, "C1"}, {EActualTyreCompound::C0, "C0"}, {EActualTyreCompound::Unknown, "Unknown"}};

    static const std::map<EVisualTyreCompound, std::string> sVisualTyreCompoundToString = {{EVisualTyreCompound::Inter, "Inter"}, {EVisualTyreCompound::Wet, "Wet"}, {EVisualTyreCompound::Soft, "Soft"}, {EVisualTyreCompound::Medium, "Medium"}, {EVisualTyreCompound::Hard, "Hard"}, {EVisualTyreCompound::Unknown, "Unknown"}};

    static const std::map<EPacketId, std::string>
        sPacketIdToString = {{EPacketId::Motion, "Motion"}, {EPacketId::Session, "Session"}, {EPacketId::LapData, "LapData"}, {EPacketId::Event, "Event"}, {EPacketId::Participants, "Participants"}, {EPacketId::CarSetups, "CarSetups"}, {EPacketId::CarTelemetry, "CarTelemetry"}, {EPacketId::CarStatus, "CarStatus"}, {EPacketId::FinalClassification, "FinalClassification"}, {EPacketId::LobbyInfo, "LobbyInfo"}, {EPacketId::CarDamage, "CarDamage"}, {EPacketId::SessionHistory, "SessionHistory"}, {EPacketId::TyreSets, "TyreSets"}, {EPacketId::MotionEx, "MotionEx"}, {EPacketId::TimeTrial, "TimeTrial"}, {EPacketId::LapPositions, "LapPositions"}};

    enum class ESessionType : uint8_t
    {
        Unknown = 0,
        P1 = 1,
        P2 = 2,
        P3 = 3,
        Short = 4,
        Q1 = 5,
        Q2 = 6,
        Q3 = 7,
        ShortQ = 8,
        OSQ = 9,
        R = 10,
        R2 = 11,
        R3 = 12,
        TimeTrial = 13
    };

    static const std::map<ESessionType, std::string>
        sSessionTypeToString = {{ESessionType::Unknown, "Unknown"}, {ESessionType::P1, "P1"}, {ESessionType::P2, "P2"}, {ESessionType::P3, "P3"}, {ESessionType::Short, "Short"}, {ESessionType::Q1, "Q1"}, {ESessionType::Q2, "Q2"}, {ESessionType::Q3, "Q3"}, {ESessionType::ShortQ, "Short Q"}, {ESessionType::OSQ, "OSQ"}, {ESessionType::R, "R"}, {ESessionType::R2, "R2"}, {ESessionType::R3, "R3"}, {ESessionType::TimeTrial, "Time Trial"}};

    enum EEventCode : uint8_t
    {
        SessionStarted = 0,
        SessionEnded = 1,
        FastestLap = 2,
        Retirement = 3,
        DRSEnabled = 4,
        DRSDisabled = 5,
        TeammateInPits = 6,
        ChequeredFlag = 7,
        RaceWinner = 8,
        PenaltyIssued = 9,
        SpeedTrapTriggered = 10,
        StartLights = 11,
        LightsOut = 12,
        DriveThroughServed = 13,
        StopGoServed = 14,
        Flashback = 15,
        ButtonStatus = 16,
        RedFlag = 17,
        Overtake = 18
    };

    static std::map<std::string, EEventCode>
        sEventCodes = {{"SSTA", EEventCode::SessionStarted}, {"SEND", EEventCode::SessionEnded}, {"FTLP", EEventCode::FastestLap}, {"RTMT", EEventCode::Retirement}, {"DRSE", EEventCode::DRSEnabled}, {"DRSD", EEventCode::DRSDisabled}, {"TMPT", EEventCode::TeammateInPits}, {"CHQF", EEventCode::ChequeredFlag}, {"RCWN", EEventCode::RaceWinner}, {"PENA", EEventCode::PenaltyIssued}, {"SPTP", EEventCode::SpeedTrapTriggered}, {"STLG", EEventCode::StartLights}, {"LGOT", EEventCode::LightsOut}, {"DTSV", EEventCode::DriveThroughServed}, {"SGSV", EEventCode::StopGoServed}, {"FLBK", EEventCode::Flashback}, {"BUTN", EEventCode::ButtonStatus}, {"RDFL", EEventCode::RedFlag}, {"OVTK", EEventCode::Overtake}};

    enum ETrackId : int8_t
    {
        Unknown = -1,
        Melbourne = 0,
        PaulRicard = 1,
        Shanghai = 2,
        SakhirBahrain = 3,
        Catalunya = 4,
        Monaco = 5,
        Montreal = 6,
        Silverstone = 7,
        Hockenheim = 8,
        Hungaroring = 9,
        Spa = 10,
        Monza = 11,
        Singapore = 12,
        Suzuka = 13,
        AbuDhabi = 14,
        Texas = 15,
        Brazil = 16,
        Austria = 17,
        Sochi = 18,
        Mexico = 19,
        BakuAzerbaijan = 20,
        SakhirShort = 21,
        SilverstoneShort = 22,
        TexasShort = 23,
        SuzukaShort = 24,
        Hanoi = 25,
        Zandvoort = 26,
        Imola = 27,
        Portimao = 28,
        Jeddah = 29,
        Miami = 30,
        LasVegas = 31,
        Losail = 32
    };

    static const std::map<ETrackId, std::string>
        sTrackIdToString = {{ETrackId::Melbourne, "Melbourne"}, {ETrackId::PaulRicard, "Paul Ricard"}, {ETrackId::Shanghai, "Shanghai"}, {ETrackId::SakhirBahrain, "Sakhir (Bahrain)"}, {ETrackId::Catalunya, "Catalunya"}, {ETrackId::Monaco, "Monaco"}, {ETrackId::Montreal, "Montreal"}, {ETrackId::Silverstone, "Silverstone"}, {ETrackId::Hockenheim, "Hockenheim"}, {ETrackId::Hungaroring, "Hungaroring"}, {ETrackId::Spa, "Spa"}, {ETrackId::Monza, "Monza"}, {ETrackId::Singapore, "Singapore"}, {ETrackId::Suzuka, "Suzuka"}, {ETrackId::AbuDhabi, "Abu Dhabi"}, {ETrackId::Texas, "Texas"}, {ETrackId::Brazil, "Brazil"}, {ETrackId::Austria, "Austria"}, {ETrackId::Sochi, "Sochi"}, {ETrackId::Mexico, "Mexico"}, {ETrackId::BakuAzerbaijan, "Baku (Azerbaijan)"}, {ETrackId::SakhirShort, "Sakhir Short"}, {ETrackId::SilverstoneShort, "Silverstone Short"}, {ETrackId::TexasShort, "Texas Short"}, {ETrackId::SuzukaShort, "SuzukaShort"}, {ETrackId::Hanoi, "Hanoi"}, {ETrackId::Zandvoort, "Zandvoort"}, {ETrackId::Imola, "Imola"}, {ETrackId::Portimao, "Portimao"}, {ETrackId::Jeddah, "Jeddah"}, {ETrackId::Miami, "Miami"}, {ETrackId::LasVegas, "Las Vegas"}, {ETrackId::Losail, "Losail"}, {ETrackId::Unknown, "Unknown"}};

    static const char *const sTeams[] = {"Mercedes", "Ferrari", "Red Bull Racing", "Williams", "Racing Point", "Renault", "Alpha-Tauri", "Haas", "McLaren", "Alfa Romeo", "McLaren 1988", "McLaren 1991", "Williams 1992", "Ferrari 1995", "Williams 1996", "McLaren 1998", "Ferrari 2002", "Ferrari 2004", "Renault 2006", "Ferrari 2007", "McLaren 2008", "Red Bull 2010", "Ferrari 1976", "Art Grand Prix", "Campos Vexatec Racing", "Carlin", "Charouz Racing System", "Dams", "Russian Time", "MP Motorsport", "Pertamina", "McLaren 1990", "Trident", "BWT Arden", "McLaren 1976", "Lotus 1972", "Ferrari 1979", "McLaren 1982", "Williams 2003", "Brawn 2009", "Lotus 1978", "F1 Generic Car", "Art GP 19", "Campos 19", "Carlin 19", "Sauber Junior Charouz 19", "Dams 19", "Uni VIRTUOSI 19", "MP Motorsport 19", "Premia 19", "Trident 19", "Arden 19", "NULL", "Benetton 1994", "Benetton 1995", "Ferrari 2000", "Jordan 1991", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "My Team"};
    static const char *const Drivers[] = {"Carlos Sainz",
                                          "Daniil Kvyar",
                                          "Daniel Ricciardo",
                                          "NULL",
                                          "NULL",
                                          "NULL",
                                          "Kimi Raikkonen",
                                          "Lewis Hamilton",
                                          "NULL",
                                          "Max Verstappen",
                                          "Nico Hulkenberg",
                                          "Kevin Magnussen",
                                          "Romain Grosjean",
                                          "Sebastian Vettel",
                                          "Sergio Perez",
                                          "Valtteri Bottas",
                                          "NULL",
                                          "Esteban Ocon",
                                          "NULL",
                                          "Lance Stroll",
                                          "Arron Barnes",
                                          "Martin Giles",
                                          "Alex Murray",
                                          "Lucas Roth",
                                          "Igor Correia",
                                          "Sophie Levasseur",
                                          "Jonas Schiffer",
                                          "Alain Forest",
                                          "Jay Letourneau",
                                          "Esto Saari",
                                          "Yasar Atiyeh",
                                          "Callisto Calabresi",
                                          "Naota Izum",
                                          "Howard Clarke",
                                          "Wilheim Kaufmann",
                                          "Marie Laursen",
                                          "Flavio Nieves",
                                          "Peter Belousov",
                                          "Klimek Michalski",
                                          "Santiago Moreno",
                                          "Benjamin Coppens",
                                          "Noah Visser",
                                          "Gert Waldmuller",
                                          "Julian Quesada",
                                          "Daniel Jones",
                                          "Artem Markelov",
                                          "Tadasuke Makino",
                                          "Sean Gelaels",
                                          "Nyck de Vries",
                                          "Jack Aitken",
                                          "George Russell",
                                          "Maximilian Gunther",
                                          "Nirei Fukuzumi",
                                          "Luca Ghiotto",
                                          "Landon Norris",
                                          "Sergio Setticamara",
                                          "Louis Deletraz",
                                          "Antonio Fuoco",
                                          "Charles Leclerc",
                                          "Pierre Gasly",
                                          "NULL",
                                          "NULL",
                                          "Alexander Albon",
                                          "Nicholas Latifi",
                                          "Dorian Boccocci",
                                          "Niko Kari",
                                          "Roberto Merhi",
                                          "Arjun Maini",
                                          "Alessio Lorandi",
                                          "Ruben Mejier",
                                          "Rashid Nair",
                                          "Jack Tremblay",
                                          "NULL",
                                          "NULL",
                                          "Antonio Giovinazzi",
                                          "Robert Kubica",
                                          "NULL",
                                          "NULL",
                                          "Nobuharu Matsushita",
                                          "Nikita Mazepin",
                                          "Guanya Zhou",
                                          "Mick Schumacher",
                                          "Callum Illiot",
                                          "Juan Manuel Correa",
                                          "Jordan King",
                                          "Mehaveer Raghunathan",
                                          "Tatiana Calderon",
                                          "Antoine Hubert",
                                          "Giuliano Alesi",
                                          "Ralph Boschung"};

    static const char *const Nationalities[] = {
        "NULL", "American", "Argentinean", "Australian",
        "Austrian", "Azerbaijani", "Bahraini", "Belgian",
        "Bolivian", "Brazilian", "British", "Bulgarian",
        "Cameroonian", "Canadian", "Chilean", "Chinese",
        "Colombian", "Costa Rican", "Croatian", "Cypriot",
        "Czech", "Danish", "Dutch", "Ecuadorean",
        "English", "Emirian", "Estonian", "Finnish",
        "French", "German", "Ghanaian", "Greek",
        "Guatemalan", "Honduran", "Hong Konger", "Hungarian",
        "Icelander", "Indian", "Indonesian", "Irish",
        "Israeli", "Italian", "Jamaican", "Japanese",
        "Jordanian", "Kuwaiti", "Latvian", "Lebanese",
        "Lithuanian", "Luxembourger", "Malaysian", "Maltese",
        "Mexican", "Monegasque", "New Zealander", "Nicaraguan",
        "North Korean", "Northern Irish", "Norwegian", "Omani",
        "Pakistani", "Panamanian", "Paraguayan", "Peruvian",
        "Polish", "Portuguese", "Qatari", "Romanian",
        "Russian", "Salvadoran", "Saudi", "Scottish",
        "Serbian", "Singaporean", "Slovakian", "Slovenian",
        "South Korean", "South African", "Spanish", "Swedish",
        "Swiss", "Thai", "Turkish", "Uruguayan",
        "Ukrainian", "Venezuelan", "Welsh", "Barbadian",
        "Vietnamese"};
}