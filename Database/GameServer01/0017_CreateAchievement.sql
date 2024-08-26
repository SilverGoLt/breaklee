CREATE TABLE Achievement (
    CharacterID INT NOT NULL,
    TotalScore INT NOT NULL DEFAULT 0,
    NormalScore INT NOT NULL DEFAULT 0,
    QuestScore INT NOT NULL DEFAULT 0,
    DungeonScore INT NOT NULL DEFAULT 0,
    ItemsScore INT NOT NULL DEFAULT 0,
    PvpScore INT NOT NULL DEFAULT 0,
    WarScore INT NOT NULL DEFAULT 0,
    HuntingScore INT NOT NULL DEFAULT 0,
    CraftScore INT NOT NULL DEFAULT 0,
    CommunityScore INT NOT NULL DEFAULT 0,
    SharedScore INT NOT NULL DEFAULT 0,
    SpecialScore INT NOT NULL DEFAULT 0,
    GeneralMemoirScore INT NOT NULL DEFAULT 0,
    SharedMemoirScore INT NOT NULL DEFAULT 0,
    DisplayTitle SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    EventTitle SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    GuildTitle SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    WarTitle SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    SlotCount INT NOT NULL DEFAULT 0,
    RewardSlotCount INT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    RewardSlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Achievement FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;