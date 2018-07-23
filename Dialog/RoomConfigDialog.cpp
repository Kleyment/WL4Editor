#include "RoomConfigDialog.h"
#include "ui_RoomConfigDialog.h"

#include <cstring>

// constexpr declarations for the initializers in the header
constexpr const char *RoomConfigDialog::TilesetNamesSetData[0x5C];
constexpr const char *RoomConfigDialog::LayerPrioritySetData[3];
constexpr const char *RoomConfigDialog::AlphaBlendAttrsSetData[12];
constexpr const char *RoomConfigDialog::Layer0MappingTypeParamSetData[2];
constexpr int RoomConfigDialog::BGLayerdataPtrsData[166];

// static variables used by RoomConfigDialog
static QStringList TilesetNamesSet, LayerPrioritySet, AlphaBlendAttrsSet, Layer0MappingTypeParamSet;
static std::vector<int> BGLayerdataPtrs[0x5C];

/// <summary>
/// Construct the instance of the RoomConfigDialog.
/// </summary>
/// <param name="parent">
/// The parent QWidget.
/// </param>
RoomConfigDialog::RoomConfigDialog(QWidget *parent, DialogParams::RoomConfigParams *CurrentRoomParams) :
    QDialog(parent),
    ui(new Ui::RoomConfigDialog)
{
    ui->setupUi(this);

    // Initialize UI elements
    ui->ComboBox_TilesetID->addItems(TilesetNamesSet);
    ui->ComboBox_LayerPriority->addItems(LayerPrioritySet);
    ui->ComboBox_AlphaBlendAttribute->addItems(AlphaBlendAttrsSet);
    ui->ComboBox_Layer0MappingType->addItems(Layer0MappingTypeParamSet);
    ui->ComboBox_TilesetID->setCurrentIndex(CurrentRoomParams->CurrentTilesetIndex);
    ui->CheckBox_Layer0Enable->setChecked(CurrentRoomParams->Layer0Enable);
    ui->CheckBox_Layer0Alpha->setChecked(CurrentRoomParams->Layer0Alpha);
    int LayerPriorityID = CurrentRoomParams->LayerPriorityAndAlphaAttr & 3;
    ui->ComboBox_LayerPriority->setCurrentIndex((LayerPriorityID < 2) ? LayerPriorityID : (LayerPriorityID - 1));
    ui->ComboBox_AlphaBlendAttribute->setCurrentIndex((CurrentRoomParams->LayerPriorityAndAlphaAttr & 0x78) >> 3);
    if(CurrentRoomParams->Layer0Enable)
    {
        ui->ComboBox_Layer0MappingType->setCurrentIndex((((CurrentRoomParams->Layer0MappingTypeParam) & 0x30) >> 4) - 1);
    }
    ui->CheckBox_Layer0AutoScroll->setChecked(CurrentRoomParams->Layer0MappingTypeParam == 0x22);
    ui->ComboBox_Layer0Picker->setEnabled(CurrentRoomParams->Layer0MappingTypeParam >= 0x20);
    ui->SpinBox_RoomWidth->setValue(CurrentRoomParams->RoomWidth);
    ui->SpinBox_RoomHeight->setValue(CurrentRoomParams->RoomHeight);
    ui->CheckBox_Layer2Enable->setChecked(CurrentRoomParams->Layer2Enable);
    ui->CheckBox_BGLayerEnable->setChecked(CurrentRoomParams->BackgroundLayerEnable);
    ui->CheckBox_BGLayerAutoScroll->setChecked(CurrentRoomParams->BackgroundLayerAutoScrollEnable);

    // Initialize the selection for the BG selection combobox
    bool CurrentBGSelectionAvailable = false;
    std::vector<int> CurrentBGLayerdataPtrs = BGLayerdataPtrs[CurrentRoomParams->CurrentTilesetIndex];
    for(unsigned int i = 0; i < CurrentBGLayerdataPtrs.size(); ++i)
    {
        if(CurrentRoomParams->BackgroundLayerDataPtr == CurrentBGLayerdataPtrs[i])
        {
            CurrentBGSelectionAvailable = true;
            ui->ComboBox_BGLayerPicker->setCurrentIndex(i);
            break;
        }
    }
    if(!CurrentBGSelectionAvailable)
    {
        ui->ComboBox_BGLayerPicker->addItem(QString::number(CurrentRoomParams->BackgroundLayerDataPtr, 16).toUpper());
        ui->ComboBox_BGLayerPicker->setCurrentIndex(ui->ComboBox_BGLayerPicker->count() - 1);
    }

    // Initialize the graphic view layers
    ui->graphicsView->infoLabel = ui->graphicViewDetailsLabel;
    int _tilesetPtr = WL4Constants::TilesetDataTable + CurrentRoomParams->CurrentTilesetIndex * 36;
    currentTileset = new LevelComponents::Tileset(_tilesetPtr, CurrentRoomParams->CurrentTilesetIndex);
    ui->graphicsView->UpdateGraphicsItems(currentTileset, CurrentRoomParams->BackgroundLayerDataPtr, CurrentRoomParams->Layer0DataPtr);

    ComboBoxInitialized = true;
}

/// <summary>
/// Deconstruct the RoomConfigDialog and clean up its instance objects on the heap.
/// </summary>
RoomConfigDialog::~RoomConfigDialog()
{
    delete ui;
}

/// <summary>
/// Get the selected config parameters based on the UI selections.
/// </summary>
/// <returns>
/// A RoomConfigParams struct containing the selected parameters from the dialog.
/// </returns>
DialogParams::RoomConfigParams RoomConfigDialog::GetConfigParams()
{
    DialogParams::RoomConfigParams configParams;

    // TODO

    return configParams;
}

/// <summary>
/// Perform static initializtion of constant data structures for the dialog.
/// </summary>
void RoomConfigDialog::StaticInitialization()
{
    // Initialize the selections for the tilesets
    for(unsigned int i = 0; i < sizeof(TilesetNamesSetData)/sizeof(TilesetNamesSetData[0]); ++i)
    {
        TilesetNamesSet << TilesetNamesSetData[i];
    }

    // Initialize the selections for the layer priority types
    for(unsigned int i = 0; i < sizeof(LayerPrioritySetData)/sizeof(LayerPrioritySetData[0]); ++i)
    {
        LayerPrioritySet << LayerPrioritySetData[i];
    }

    // Initialize the selections for the alpha blending types
    for(unsigned int i = 0; i < sizeof(AlphaBlendAttrsSetData)/sizeof(AlphaBlendAttrsSetData[0]); ++i)
    {
        AlphaBlendAttrsSet << AlphaBlendAttrsSetData[i];
    }

    // Initialize the selections for the types of layer mapping for layer 0
    for(unsigned int i = 0; i < sizeof(Layer0MappingTypeParamSetData)/sizeof(Layer0MappingTypeParamSetData[0]); ++i)
    {
        Layer0MappingTypeParamSet << Layer0MappingTypeParamSetData[i];
    }

    // Initialize the selections for the tilesets's available BGs
    for(unsigned int i = 0, idx = 0; idx < sizeof(BGLayerdataPtrsData)/sizeof(BGLayerdataPtrsData[0]); ++i)
    {
        std::vector<int> vec;
        int count = BGLayerdataPtrsData[idx++];
        while(count--)
        {
            vec.push_back(BGLayerdataPtrsData[idx++]);
        }
        BGLayerdataPtrs[i] = vec;
    }
}

/// <summary>
/// Slot function for CheckBox_Layer0Enable_stateChanged.
/// </summary>
void RoomConfigDialog::on_CheckBox_Layer0Enable_stateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->CheckBox_Layer0Alpha->setEnabled(true);
        ui->ComboBox_Layer0MappingType->setEnabled(true);
    }
    else
    {
        ui->CheckBox_Layer0Alpha->setChecked(false);
        ui->CheckBox_Layer0Alpha->setEnabled(false);
        ui->ComboBox_Layer0MappingType->setEnabled(false);
    }
}

/// <summary>
/// Slot function for CheckBox_Layer0Alpha_stateChanged.
/// </summary>
void RoomConfigDialog::on_CheckBox_Layer0Alpha_stateChanged(int state)
{
    ui->ComboBox_AlphaBlendAttribute->setEnabled(ui->CheckBox_Layer0Alpha->isChecked());
    if(state == Qt::Unchecked)
    {
        ui->ComboBox_AlphaBlendAttribute->setCurrentIndex(0);
    }
}

/// <summary>
/// Slot function for ComboBox_Layer0MappingType_currentIndexChanged.
/// </summary>
void RoomConfigDialog::on_ComboBox_Layer0MappingType_currentIndexChanged(int index)
{
    if(ComboBoxInitialized)
    {
        if(!index)
        {
            ui->CheckBox_Layer0AutoScroll->setChecked(false);
            ui->CheckBox_Layer0AutoScroll->setEnabled(false);
            ui->ComboBox_Layer0Picker->setEnabled(false);
        }
        else
        {
            ui->CheckBox_Layer0AutoScroll->setEnabled(true);
            ui->ComboBox_Layer0Picker->setEnabled(true);
        }
    }
}

/// <summary>
/// Slot function for ComboBox_TilesetID_currentIndexChanged.
/// </summary>
void RoomConfigDialog::on_ComboBox_TilesetID_currentIndexChanged(int index)
{
    if(ComboBoxInitialized)
    {
        // Update the graphic view
        LevelComponents::Tileset *oldTileset = currentTileset;
        int _tilesetPtr = WL4Constants::TilesetDataTable + index * 36;
        currentTileset = new LevelComponents::Tileset(_tilesetPtr, index);
        delete oldTileset;

        // Update the available BG layers to choose from
        std::vector<int> BGlayers = BGLayerdataPtrs[index];
        ui->ComboBox_BGLayerPicker->clear();
        QStringList elements;
        if(BGlayers.size())
        {
            for(auto iter = BGlayers.begin(); iter != BGlayers.end(); ++iter)
            {
                elements << QString::number(*iter, 16).toUpper();
            }
        }
        else
        {
            elements << QString::number(WL4Constants::BGLayerDisableDefaultPtr, 16).toUpper();
        }
        ui->ComboBox_BGLayerPicker->addItems(elements);

        int BGptr = ui->ComboBox_BGLayerPicker->currentText().toUInt(nullptr, 16);
        int L0ptr = ui->ComboBox_Layer0Picker->currentText().toUInt(nullptr, 16);
        ui->graphicsView->UpdateGraphicsItems(currentTileset, BGptr, L0ptr);
    }
}

/// <summary>
/// Slot function for CheckBox_BGLayerEnable_stateChanged.
/// </summary>
void RoomConfigDialog::on_CheckBox_BGLayerEnable_stateChanged(int state)
{
    int tilesetIndex = ui->ComboBox_TilesetID->currentIndex();
    ui->ComboBox_BGLayerPicker->setEnabled(state == Qt::Checked && BGLayerdataPtrs[tilesetIndex].size());
    ui->CheckBox_BGLayerAutoScroll->setEnabled(state == Qt::Checked);
    if(ui->ComboBox_BGLayerPicker->count() > 0)
    {
        for(int i = ui->ComboBox_BGLayerPicker->count() - 1; i >= 0 ; ++i)
        {
            ui->ComboBox_BGLayerPicker->removeItem(i);
        }
    }
    for(unsigned int i = 0; i < BGLayerdataPtrs[tilesetIndex].size(); ++i)
    {
        if(BGLayerdataPtrs[tilesetIndex][i])
        {
            ui->ComboBox_BGLayerPicker->addItem(QString::number(BGLayerdataPtrs[tilesetIndex][i], 16).toUpper());
        }
    }
}

/// <summary>
/// Slot function for ComboBox_BGLayerPicker_currentIndexChanged.
/// </summary>
void RoomConfigDialog::on_ComboBox_BGLayerPicker_currentIndexChanged(int index)
{
    (void) index;
    if(ComboBoxInitialized)
    {
        int BGptr = ui->ComboBox_BGLayerPicker->currentText().toUInt(nullptr, 16);
        int L0ptr = ui->ComboBox_Layer0Picker->currentText().toUInt(nullptr, 16);
        ui->graphicsView->UpdateGraphicsItems(currentTileset, BGptr, L0ptr);
    }
}

/// <summary>
/// Slot function for ComboBox_Layer0Picker_currentIndexChanged.
/// </summary>
void RoomConfigDialog::on_ComboBox_Layer0Picker_currentIndexChanged(int index)
{
    (void) index;
    if(ComboBoxInitialized)
    {
        ui->CheckBox_Layer0AutoScroll->setEnabled(true);
        int BGptr = ui->ComboBox_BGLayerPicker->currentText().toUInt(nullptr, 16);
        int L0ptr = ui->ComboBox_Layer0Picker->currentText().toUInt(nullptr, 16);
        ui->graphicsView->UpdateGraphicsItems(currentTileset, BGptr, L0ptr);
    }
}
