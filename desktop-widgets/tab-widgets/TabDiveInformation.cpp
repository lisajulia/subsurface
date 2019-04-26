// SPDX-License-Identifier: GPL-2.0
#include "TabDiveInformation.h"
#include "ui_TabDiveInformation.h"
#include "../tagwidget.h"

#include <core/qthelper.h>
#include <core/statistics.h>
#include <core/display.h>

TabDiveInformation::TabDiveInformation(QWidget *parent) : TabBase(parent), ui(new Ui::TabDiveInformation())
{
	ui->setupUi(this);
}

TabDiveInformation::~TabDiveInformation()
{
	delete ui;
}

void TabDiveInformation::clear()
{
	ui->sacText->clear();
	ui->otuText->clear();
	ui->maxcnsText->clear();
	ui->oxygenHeliumText->clear();
	ui->gasUsedText->clear();
	ui->dateText->clear();
	ui->diveTimeText->clear();
	ui->surfaceIntervalText->clear();
	ui->maximumDepthText->clear();
	ui->averageDepthText->clear();
	ui->waterTemperatureText->clear();
	ui->airTemperatureText->clear();
	ui->airPressureText->clear();
	ui->salinityText->clear();
}

void TabDiveInformation::updateData()
{
	if (!current_dive) {
		clear();
		return;
	}

	ui->maxcnsText->setText(QString("%1\%").arg(current_dive->maxcns));
	ui->otuText->setText(QString("%1").arg(current_dive->otu));
	ui->maximumDepthText->setText(get_depth_string(current_dive->maxdepth, true));
	ui->averageDepthText->setText(get_depth_string(current_dive->meandepth, true));
	ui->dateText->setText(get_short_dive_date_string(current_dive->when));
	ui->waterTemperatureText->setText(get_temperature_string(current_dive->watertemp, true));
	ui->airTemperatureText->setText(get_temperature_string(current_dive->airtemp, true));

	volume_t gases[MAX_CYLINDERS] = {};
	get_gas_used(current_dive, gases);
	QString volumes;
	int mean[MAX_CYLINDERS], duration[MAX_CYLINDERS];
	per_cylinder_mean_depth(current_dive, select_dc(current_dive), mean, duration);
	volume_t sac;
	QString gaslist, SACs, separator;

	gaslist = ""; SACs = ""; volumes = ""; separator = "";
	for (int i = 0; i < MAX_CYLINDERS; i++) {
		if (!is_cylinder_used(current_dive, i))
			continue;
		gaslist.append(separator); volumes.append(separator); SACs.append(separator);
		separator = "\n";

		gaslist.append(gasname(current_dive->cylinder[i].gasmix));
		if (!gases[i].mliter)
			continue;
		volumes.append(get_volume_string(gases[i], true));
		if (duration[i]) {
			sac.mliter = lrint(gases[i].mliter / (depth_to_atm(mean[i], current_dive) * duration[i] / 60));
			SACs.append(get_volume_string(sac, true).append(tr("/min")));
		}
	}
	ui->gasUsedText->setText(volumes);
	ui->oxygenHeliumText->setText(gaslist);

	ui->diveTimeText->setText(get_dive_duration_string(current_dive->duration.seconds, tr("h"), tr("min"), tr("sec"),
			" ", current_dive->dc.divemode == FREEDIVE));

	timestamp_t surface_interval = get_surface_interval(current_dive->when);
	if (surface_interval >= 0)
		ui->surfaceIntervalText->setText(get_dive_surfint_string(surface_interval, tr("d"), tr("h"), tr("min")));
	else
		ui->surfaceIntervalText->clear();

	ui->sacText->setText( mean[0] ? SACs : QString());

	if (current_dive->surface_pressure.mbar) 	/* this is ALWAYS displayed in mbar */
		ui->airPressureText->setText(QString("%1mbar").arg(current_dive->surface_pressure.mbar));
	else
		ui->airPressureText->clear();

	if (current_dive->salinity)
		ui->salinityText->setText(QString("%1g/ℓ").arg(current_dive->salinity / 10.0));
	else
		ui->salinityText->clear();

}
