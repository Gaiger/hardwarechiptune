#include <QGridLayout>
#include <QShortcut>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>

#include <QDebug>
#include "ui_HardwareChiptunePanelWidget.h"
#include "HardwareChiptunePanelWidget.h"


void ReplaceWidget(QWidget *p_widget, QWidget *p_replaced_widget)
{
	QGridLayout *p_layout = new QGridLayout(p_replaced_widget);
	p_layout->addWidget(p_widget, 0, 0);
	p_layout->setContentsMargins(0, 0, 0, 0);
	p_layout->setSpacing(0);
}

/**********************************************************************************/

HardwareChiptunePanelWidget::HardwareChiptunePanelWidget(AudioPlayer *p_player, QWidget *parent) :
	QWidget(parent),
	m_p_player(p_player),

	ui(new Ui::HardwareChiptunePanelWidget)
{
	ui->setupUi(this);
	QFont font20("Monospace");
	font20.setStyleHint(QFont::TypeWriter);
	font20.setPixelSize(20);

	do{
		m_p_song_plaintextedit = new SongPlainTextEdit(m_p_player->GetTuneManager(), this);
		ReplaceWidget(m_p_song_plaintextedit, ui->SongWidget);

		ui->SongIndexSpinBox->setFont(font20);
		ui->SongPlayPushButton->setToolTip("ctrl + p");
		ui->SongApplyPushButton->setToolTip("ctrl + l");

		QObject::connect(m_p_song_plaintextedit, &QPlainTextEdit::modificationChanged, this,
						 [&](bool is_changed){
			ui->SongPlayPushButton->setEnabled(!is_changed);
			ui->SongApplyPushButton->setEnabled(is_changed);
		});

		QObject::connect(m_p_song_plaintextedit, &SongPlainTextEdit::ParseScoresErrorOccurred,
						 ui->ErrorMessageLabel, &QLabel::setText);
	}while(0);

	do{
		m_p_track_plaintextedit = new TrackPlainTextEdit(m_p_player->GetTuneManager(), this);
		ReplaceWidget(m_p_track_plaintextedit, ui->TrackWidget);

		ui->TrackIndexSpinBox->setFont(font20);
		ui->TrackPlayPushButton->setToolTip("ctrl + p");
		ui->TrackApplyPushButton->setToolTip("ctrl + l");

		QObject::connect(m_p_track_plaintextedit, &QPlainTextEdit::modificationChanged, this,
						 [&](bool is_changed){
			ui->TrackPlayPushButton->setEnabled(!is_changed);
			ui->TrackApplyPushButton->setEnabled(is_changed);
		});

		QObject::connect(m_p_track_plaintextedit, &TrackPlainTextEdit::ParseMeasureErrorOccurred,
						 ui->ErrorMessageLabel, &QLabel::setText);
	}while(0);

	do{

		m_p_instrument_plaintextedit = new InstrumentPlainTextEdit(m_p_player->GetTuneManager(), this);
		ReplaceWidget(m_p_instrument_plaintextedit, ui->InstrumentWidget);

		ui->InstrumentIndexSpinBox->setFont(font20);
		ui->InstrumentApplyPushButton->setToolTip("ctrl + l");

		QObject::connect(m_p_instrument_plaintextedit, &QPlainTextEdit::modificationChanged, this,
						 [&](bool is_changed){
			ui->InstrumentApplyPushButton->setEnabled(is_changed);
			ui->SongPlayPushButton->setEnabled(!is_changed);
			ui->TrackPlayPushButton->setEnabled(!is_changed);
		});

		QObject::connect(m_p_instrument_plaintextedit, &InstrumentPlainTextEdit::ParseTimbreErrorOccurred,
						 ui->ErrorMessageLabel, &QLabel::setText);
	}while(0);

	do
	{
		QChart *p_chart = new QChart();
		m_p_wave_chartview = new WaveChartView(p_chart, this);
		ReplaceWidget(m_p_wave_chartview, ui->WaveWidget);
	}while(0);

	ui->ErrorMessageLabel->setFont(font20);

	QObject::connect(p_player->GetTuneManager(), &TuneManager::GeneratingSongStateChanged,
					 this, &HardwareChiptunePanelWidget::HandleGeneratingSongStateChanged, Qt::QueuedConnection);

	QObject::connect(p_player->GetTuneManager(), &TuneManager::GeneratingTrackStateChanged,
					 this, &HardwareChiptunePanelWidget::HandleGeneratingTrackStateChanged, Qt::QueuedConnection);

	QObject::connect(p_player->GetTuneManager(), &TuneManager::WaveFetched,
					 this, &HardwareChiptunePanelWidget::HandleWaveFetched, Qt::QueuedConnection);

	QObject::connect(p_player->GetTuneManager(), &TuneManager::LightChanged,
					 this, &HardwareChiptunePanelWidget::HandleLightChanged);

	QShortcut *p_shortcut;
	p_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
	QObject::connect(p_shortcut, &QShortcut::activated,
					 this, &HardwareChiptunePanelWidget::HandleShortcut_CTRL_L_Activated);

	p_shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this);
	QObject::connect(p_shortcut, &QShortcut::activated,
					 this, &HardwareChiptunePanelWidget::HandleShortcut_CTRL_P_Activated);

	do
	{
		if(0 != m_p_player->GetTuneManager()->LoadSongFile("test2.song")){
			break;
		}
		HardwareChiptunePanelWidget::UpdateContents();
	}while(0);
}

/**********************************************************************************/

HardwareChiptunePanelWidget::~HardwareChiptunePanelWidget()
{
	m_p_player->Stop();

	delete m_p_song_plaintextedit;
	delete m_p_track_plaintextedit;
	delete m_p_instrument_plaintextedit;

	delete ui;
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::UpdateContents(void)
{

	do{
		TuneManager::songline *p_songlines;
		int number_of_songlines;
		m_p_player->GetTuneManager()->GetSongLines(&p_songlines, &number_of_songlines);

		ui->SongIndexSpinBox->blockSignals(true);
		ui->SongIndexSpinBox->setRange(0, number_of_songlines - 1);
		ui->SongIndexSpinBox->setValue(0);
		ui->SongIndexSpinBox->setEnabled(true);
		ui->SongIndexSpinBox->blockSignals(false);

		m_p_song_plaintextedit->ShowSong();
	}while(0);

	do
	{
		TuneManager::track *p_track;
		int number_of_tracks;
		int track_length;
		m_p_player->GetTuneManager()->GetTracks(&p_track, &number_of_tracks, &track_length);
		ui->TrackIndexSpinBox->setRange(0 + 1, number_of_tracks - 1);
		int spinbox_value = ui->TrackIndexSpinBox->value();
		if(ui->TrackIndexSpinBox->minimum() > spinbox_value
				|| ui->TrackIndexSpinBox->maximum() < spinbox_value){
			spinbox_value = 1;
		}
		ui->TrackIndexSpinBox->setValue(spinbox_value);
		ui->TrackIndexSpinBox->setEnabled(true);

		m_p_track_plaintextedit->ShowTrack(spinbox_value);
	}while(0);

	do
	{
		TuneManager::instrument *p_instruments;
		int number_of_instruments;
		m_p_player->GetTuneManager()->GetInstruments(&p_instruments, &number_of_instruments);
		ui->InstrumentIndexSpinBox->setRange(0 + 1, number_of_instruments - 1);
		int spinbox_value = ui->InstrumentIndexSpinBox->value();
		if(ui->InstrumentIndexSpinBox->minimum() > spinbox_value
				|| ui->InstrumentIndexSpinBox->maximum() < spinbox_value){
			spinbox_value = 1;
		}
		ui->InstrumentIndexSpinBox->setValue(spinbox_value);
		ui->InstrumentIndexSpinBox->setEnabled(true);

		m_p_instrument_plaintextedit->ShowInstrument(spinbox_value);
	}while(0);

	m_p_wave_chartview->Reset();
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::timerEvent(QTimerEvent *p_event)
{
	QWidget::timerEvent(p_event);
}

/**********************************************************************************/

#define UNICODE_PLAY_ICON						u8"\u25B7"
#define UNICODE_STOP_ICON						u8"\u25A1"

void HardwareChiptunePanelWidget::HandleGeneratingSongStateChanged(bool is_generating, int generating_song_index)
{

	do
	{
		if(true == is_generating){
			ui->SongPlayPushButton->setText(QString(UNICODE_STOP_ICON));
			ui->SongIndexSpinBox->blockSignals(true);
			ui->SongIndexSpinBox->setEnabled(false);
			int playing_song_index = generating_song_index - 1;
			ui->SongIndexSpinBox->setValue(playing_song_index);
			ui->SongIndexSpinBox->blockSignals(false);
			break;
		}

		ui->SongPlayPushButton->setText(QString(UNICODE_PLAY_ICON));
		ui->SongIndexSpinBox->setEnabled(true);
		m_p_wave_chartview->CleanUndrawnWave();
	}while(0);

}

/**********************************************************************************/

void HardwareChiptunePanelWidget::HandleGeneratingTrackStateChanged(bool is_generating, int generating_track_index, int generating_line_index)
{
	Q_UNUSED(generating_track_index);
	Q_UNUSED(generating_line_index);
	do
	{
		if(true == is_generating){
			ui->TrackPlayPushButton->setText(QString(UNICODE_STOP_ICON));
			break;
		}
		ui->TrackPlayPushButton->setText(QString(UNICODE_PLAY_ICON));
		m_p_wave_chartview->CleanUndrawnWave();
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::HandleLightChanged(int light_index, bool is_turn_on)
{
	QString text_string = "background-color: rgb(36, 36, 36);";
	if(true == is_turn_on){
		text_string = "background-color: rgb(72, 72, 72);";
	}

	do
	{

		if(0 == light_index){
			ui->LightWidget0->setStyleSheet(text_string);
			break;
		}
		ui->LightWidget1->setStyleSheet(text_string);
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::HandleWaveFetched(const QByteArray wave_bytearray)
{
	//qDebug() << Q_FUNC_INFO <<  wave_bytearray.size();
	m_p_wave_chartview->GiveWave(wave_bytearray);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::HandleShortcut_CTRL_L_Activated(void)
{
	do
	{
		if(true == m_p_song_plaintextedit->hasFocus()){
			if(ui->SongApplyPushButton->isEnabled()){
				ui->SongApplyPushButton->click();
			}
			break;
		}

		if(true == m_p_track_plaintextedit->hasFocus()){
			if(ui->TrackApplyPushButton->isEnabled()){
				ui->TrackApplyPushButton->click();
			}
			break;
		}

		if(true == m_p_instrument_plaintextedit->hasFocus()){
			if(ui->InstrumentApplyPushButton->isEnabled()){
				ui->InstrumentApplyPushButton->click();
			}
			break;
		}
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::HandleShortcut_CTRL_P_Activated(void)
{
	do
	{
		if(true == m_p_song_plaintextedit->hasFocus()
				|| true == ui->SongPlayPushButton->hasFocus()){
			if(ui->SongPlayPushButton->isEnabled()){
				ui->SongPlayPushButton->click();
			}
			break;
		}

		if(true == m_p_track_plaintextedit->hasFocus()
				|| true == ui->TrackPlayPushButton->hasFocus()){
			if(ui->TrackPlayPushButton->isEnabled()){
				ui->TrackPlayPushButton->click();
			}
			break;
		}
	}while(0);
}

/**********************************************************************************/

void  HardwareChiptunePanelWidget::on_OpenFilePushButton_released(void)
{
	QString load_filename_string = QFileDialog::getOpenFileName(this, QString("Open the Song File"),
											   QString(),
											   QString("Song (*.song);;"
													   "Text Files (*.txt);;"
													   "All file (*)")
																);
	do
	{
		if(true == load_filename_string.isNull()){
			break;
		}
		m_p_player->Stop();
		int ret = m_p_player->GetTuneManager()->LoadSongFile(load_filename_string);
		if(0 != ret){
			QString error_string("File is not found.");
			if(-2 == ret){
				error_string = QString("file format wrong.");
			}
			QMessageBox::critical(this, "Load File Error", error_string);
			break;
		}
		HardwareChiptunePanelWidget::UpdateContents();
	}while(0);
}

/**********************************************************************************/

void  HardwareChiptunePanelWidget::on_SaveFilePushButton_released(void)
{
	QString suggested_filename_string = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
	suggested_filename_string += ".song";
	QString save_filename_string = QFileDialog::getSaveFileName(this, QString("Save the Song File"),
											   suggested_filename_string,
											   QString("Song (*.song);;"
													   "Text Files (*.txt);;"
													   "All file (*)")
																);

	do
	{
		if(true == save_filename_string.isNull()){
			break;
		}
		m_p_player->GetTuneManager()->SaveSongFile(save_filename_string);
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_ImportChunkDataPushButton_released(void)
{
	QString load_filename_string = QFileDialog::getOpenFileName(this, QString("Import the Binary Data File"),
											   QString(),
											   QString("binary data (*.data *.bin *.hex);;"
													   "All file (*)")
																);
	do
	{
		if(true == load_filename_string.isNull()){
			break;
		}
		m_p_player->Stop();
		int ret = m_p_player->GetTuneManager()->ImportChunkDataFile(load_filename_string);
		if(0 != ret){
			QString error_string("File is not found.");
			if(-2 == ret){
				error_string = QString("file format wrong.");
			}
			QMessageBox::critical(this, "Load File Error", error_string);
			break;
		}
		HardwareChiptunePanelWidget::UpdateContents();
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_ExportChunkDataPushButton_released(void)
{
	QString suggested_filename_string = QString("songdata");
	QString filename_string = QFileDialog::getSaveFileName(this, QString("Export the Data File"),
																  suggested_filename_string,
																  QString("binary data (*.data *.bin *.hex);;"
																		  "C source code(*.c);;"
																		  "AVR assembler data and C header (*.s *.h);;"
																		  "All file (*)")
														   );

	do
	{
		if(true == filename_string.isNull()){
			break;
		}

		QFileInfo fileinfo(filename_string);
		TuneManager::EXPORT_TYPE export_type = TuneManager::TEXT;
		do
		{
			if(QString("s") == fileinfo.suffix()){
				export_type = TuneManager::AVR_ASM_AND_C_HEADER;
				break;
			}

			if(QString("c") == fileinfo.suffix()){
				export_type = TuneManager::C_SOURCECODE;
				break;
			}

			if(QString("data") == fileinfo.suffix()
					|| QString("bin") == fileinfo.suffix()
					|| QString("hex") == fileinfo.suffix()){
				export_type = TuneManager::BINARY_DATA;
				break;
			}
		}while(0);

		int ret = m_p_player->GetTuneManager()->ExportChunkDataFile(filename_string, export_type);
		if(0 != ret){
			QString error_string("error occurs in file saving.");
			QMessageBox::critical(this, "Export File Error", error_string);
		}
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_SongIndexSpinBox_valueChanged(int i)
{
	Q_UNUSED(i);
	m_p_wave_chartview->Reset();
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_SongPlayPushButton_released(void)
{
	do
	{
		if( ui->SongPlayPushButton->text() == QString(UNICODE_PLAY_ICON)){
			m_p_player->PlaySong( ui->SongIndexSpinBox->value());
			ui->SongPlayPushButton->setText(QString(UNICODE_STOP_ICON));
			ui->SongIndexSpinBox->setEnabled(false);
			break;
		}

		m_p_player->Stop();
		ui->SongPlayPushButton->setText(QString(UNICODE_PLAY_ICON));
		ui->SongIndexSpinBox->setEnabled(true);
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_SongApplyPushButton_released(void)
{
	if(0 != m_p_song_plaintextedit->UpdateScores()){
		return ;
	}

	ui->SongApplyPushButton->setEnabled(false);
	ui->ErrorMessageLabel->setText("");
	HardwareChiptunePanelWidget::UpdateContents();
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_TrackIndexSpinBox_valueChanged(int i)
{
	int tune_type;
	bool is_playing = m_p_player->GetTuneManager()->IsGeneratingWave(&tune_type);
	if(true == is_playing && tune_type == TuneManager::TRACK){
		m_p_player->Stop();
	}

	m_p_track_plaintextedit->ShowTrack(i);
	m_p_wave_chartview->Reset();
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_TrackPlayPushButton_released(void)
{
	do
	{
		if( ui->TrackPlayPushButton->text() == QString(UNICODE_PLAY_ICON)){
			int generating_track_index;
			generating_track_index = ui->TrackIndexSpinBox->value();
			m_p_player->PlayTrack(ui->TrackIndexSpinBox->value());
			ui->TrackPlayPushButton->setText(QString(UNICODE_STOP_ICON));
			break;
		}
		m_p_player->Stop();
		ui->TrackPlayPushButton->setText(QString(UNICODE_PLAY_ICON));
	}while(0);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_TrackApplyPushButton_released(void)
{
	if(0 != m_p_track_plaintextedit->UpdateMeasure()){
		return ;
	}
	ui->InstrumentApplyPushButton->setEnabled(false);
	ui->ErrorMessageLabel->setText("");
	//HardwareChiptunePanelWidget::UpdateContents();
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_InstrumentIndexSpinBox_valueChanged(int i)
{
	m_p_instrument_plaintextedit->ShowInstrument(i);
}

/**********************************************************************************/

void HardwareChiptunePanelWidget::on_InstrumentApplyPushButton_released(void)
{
	if(0 != m_p_instrument_plaintextedit->UpdateTimbre()){
		return ;
	}
	ui->InstrumentApplyPushButton->setEnabled(false);
	ui->ErrorMessageLabel->setText("");
	//HardwareChiptunePanelWidget::UpdateContents();
}

