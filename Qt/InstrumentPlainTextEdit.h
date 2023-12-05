#ifndef INSTRUMENTPLAINTEXTEDIT_H
#define INSTRUMENTPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <TuneManager.h>

class InstrumentPlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit InstrumentPlainTextEdit(TuneManager *p_tune_manager, QWidget *parent = nullptr);

	void ShowInstrument(int index, bool is_clean_undoredostack = true);
	int UpdateTimbre(void);
public:
	signals:
	void ParseTimbreErrorOccurred(const QString &error_string);
private slots:
	void HandleGeneratingSongStateChanged(bool is_generating, int generating_song_index);
	void HandleGeneratingTrackStateChanged(bool is_generating, int generating_track_index, int generating_line_index);

private:
	int ParseDocument(void);
	int ParseInstrlineString(QString cmd_string, QString parameter_string, TuneManager::instrline *p_instrline);
private:
	TuneManager *m_p_tune_manager;
	int m_current_shown_index;
};

#endif // INSTRUMENTPLAINTEXTEDIT_H
