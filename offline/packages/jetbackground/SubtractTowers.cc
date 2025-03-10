#include "SubtractTowers.h"

#include "TowerBackground.h"

// sPHENIX includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerv1.h>

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
#include <phool/phool.h>

// standard includes
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

SubtractTowers::SubtractTowers(const std::string &name)
  : SubsysReco(name)
{
}

int SubtractTowers::InitRun(PHCompositeNode *topNode)
{
  CreateNode(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int SubtractTowers::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
  {
    std::cout << "SubtractTowers::process_event: entering, with _use_flow_modulation = " << _use_flow_modulation << std::endl;
  }

  // pull out the tower containers and geometry objects at the start
  RawTowerContainer *towersEM3 = nullptr;
  RawTowerContainer *towersIH3 = nullptr;
  RawTowerContainer *towersOH3 = nullptr;
  TowerInfoContainer *towerinfosEM3 = nullptr;
  TowerInfoContainer *towerinfosIH3 = nullptr;
  TowerInfoContainer *towerinfosOH3 = nullptr;

  if (m_use_towerinfo)
  {
    EMTowerName = m_towerNodePrefix + "_CEMC_RETOWER";
    IHTowerName = m_towerNodePrefix + "_HCALIN";
    OHTowerName = m_towerNodePrefix + "_HCALOUT";
    towerinfosEM3 = findNode::getClass<TowerInfoContainer>(topNode, EMTowerName);
    towerinfosIH3 = findNode::getClass<TowerInfoContainer>(topNode, IHTowerName);
    towerinfosOH3 = findNode::getClass<TowerInfoContainer>(topNode, OHTowerName);

    if (Verbosity() > 0)
    {
      std::cout << "SubtractTowers::process_event: " << towerinfosEM3->size() << EMTowerName << " towers" << std::endl;
      std::cout << "SubtractTowers::process_event: " << towerinfosIH3->size() << IHTowerName << " towers" << std::endl;
      std::cout << "SubtractTowers::process_event: " << towerinfosOH3->size() << OHTowerName << " towers" << std::endl;
    }
  }
  else
  {
    towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
    towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
    towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");

    if (Verbosity() > 0)
    {
      std::cout << "SubtractTowers::process_event: " << towersEM3->size() << " TOWER_CALIB_CEMC_RETOWER towers" << std::endl;
      std::cout << "SubtractTowers::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << std::endl;
      std::cout << "SubtractTowers::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;
    }
  }

  RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  // these should have already been created during InitRun()
  RawTowerContainer *emcal_towers = nullptr;
  RawTowerContainer *ihcal_towers = nullptr;
  RawTowerContainer *ohcal_towers = nullptr;
  TowerInfoContainer *emcal_towerinfos = nullptr;
  TowerInfoContainer *ihcal_towerinfos = nullptr;
  TowerInfoContainer *ohcal_towerinfos = nullptr;
  if (m_use_towerinfo)
  {
    EMTowerName = m_towerNodePrefix + "_CEMC_RETOWER_SUB1";
    IHTowerName = m_towerNodePrefix + "_HCALIN_SUB1";
    OHTowerName = m_towerNodePrefix + "_HCALOUT_SUB1";
    emcal_towerinfos = findNode::getClass<TowerInfoContainer>(topNode, EMTowerName);
    ihcal_towerinfos = findNode::getClass<TowerInfoContainer>(topNode, IHTowerName);
    ohcal_towerinfos = findNode::getClass<TowerInfoContainer>(topNode, OHTowerName);
  }
  if (Verbosity() > 0)
  {
    std::cout << "SubtractTowers::process_event: starting with " << emcal_towerinfos->size() << EMTowerName << " towers" << std::endl;
    std::cout << "SubtractTowers::process_event: starting with " << ihcal_towerinfos->size() << IHTowerName << " towers" << std::endl;
    std::cout << "SubtractTowers::process_event: starting with " << ohcal_towerinfos->size() << OHTowerName << " towers" << std::endl;
  }

  else
  {
    emcal_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
    ihcal_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
    ohcal_towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
    if (Verbosity() > 0)
    {
      std::cout << "SubtractTowers::process_event: starting with " << emcal_towers->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: starting with " << ihcal_towers->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: starting with " << ohcal_towers->size() << " TOWER_CALIB_HCALOUT_SUB1 towers" << std::endl;
    }
  }

  TowerBackground *towerbackground;
  if (m_use_towerinfo)
  {
    towerbackground = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  }
  else
  {
    towerbackground = findNode::getClass<TowerBackground>(topNode, "TowerBackground_Sub2");
  }
  // read these in to use, even if we don't use flow modulation in the subtraction
  float background_v2 = towerbackground->get_v2();
  float background_Psi2 = towerbackground->get_Psi2();

  // EMCal

  // replicate existing towers
  if (m_use_towerinfo)
  {
    unsigned int nchannels_em = towerinfosEM3->size();
    for (unsigned int channel = 0; channel < nchannels_em; channel++)
    {
      TowerInfo *tower = towerinfosEM3->get_tower_at_channel(channel);
      unsigned int towerkey = towerinfosEM3->encode_key(channel);
      int ieta = towerinfosEM3->getTowerEtaBin(towerkey);
      int iphi = towerinfosEM3->getTowerPhiBin(towerkey);
      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(0).at(ieta);
      if (_use_flow_modulation)
      {
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        float tower_phi = geomIH->get_tower_geometry(key)->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      // if a tower is masked, leave it at zero
      if (tower->get_isHot() || tower->get_isNoCalib() || tower->get_isNotInstr() || tower->get_isBadChi2())
      {
        new_energy = 0;
      }

      emcal_towerinfos->get_tower_at_channel(channel)->set_time(tower->get_time());
      emcal_towerinfos->get_tower_at_channel(channel)->set_energy(new_energy);

      if (Verbosity() > 5)
      {
        std::cout << " SubtractTowers::process_event : EMCal tower at ieta / iphi = " << ieta << " / " << iphi << ", pre-sub / after-sub E = " << raw_energy << " / " << new_energy << std::endl;
      }
    }
  }
  else
  {
    RawTowerContainer::ConstRange begin_end_EM = towersEM3->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end_EM.first; rtiter != begin_end_EM.second; ++rtiter)
    {
      RawTower *tower = rtiter->second;

      int this_etabin = tower->get_bineta();
      int this_phibin = tower->get_binphi();
      float this_E = tower->get_energy();

      RawTower *new_tower = new RawTowerv1();
      new_tower->set_energy(this_E);
      emcal_towers->AddTower(this_etabin, this_phibin, new_tower);
    }

    // now fill in additional towers with zero energy to fill out the full grid
    // but note: after retowering, all of these should already exist...
    for (int eta = 0; eta < geomIH->get_etabins(); eta++)
    {
      for (int phi = 0; phi < geomIH->get_phibins(); phi++)
      {
        if (!emcal_towers->getTower(eta, phi))
        {
          RawTower *new_tower = new RawTowerv1();
          new_tower->set_energy(0);
          emcal_towers->AddTower(eta, phi, new_tower);
        }
      }
    }
    // update towers for background subtraction...
    for (RawTowerContainer::ConstIterator rtiter = emcal_towers->getTowers().first; rtiter != emcal_towers->getTowers().second; ++rtiter)
    {
      RawTower *tower = rtiter->second;
      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(0).at(tower->get_bineta());
      if (_use_flow_modulation)
      {
        float tower_phi = geomIH->get_tower_geometry(tower->get_key())->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      if (raw_energy == 0)
      {
        new_energy = 0;
      }
      tower->set_energy(new_energy);
      if (Verbosity() > 5)
      {
        std::cout << "SubtractTowers::process_event : EMCal tower at eta / phi = " << tower->get_bineta() << " / " << tower->get_binphi() << ", pre-sub / after-sub E = " << raw_energy << " / " << tower->get_energy() << std::endl;
      }
    }
  }
  // IHCal
  // replicate existing towers
  if (m_use_towerinfo)
  {
    unsigned int nchannels_ih = towerinfosIH3->size();
    for (unsigned int channel = 0; channel < nchannels_ih; channel++)
    {
      TowerInfo *tower = towerinfosIH3->get_tower_at_channel(channel);
      unsigned int towerkey = towerinfosIH3->encode_key(channel);
      int ieta = towerinfosIH3->getTowerEtaBin(towerkey);
      int iphi = towerinfosIH3->getTowerPhiBin(towerkey);

      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(1).at(ieta);
      if (_use_flow_modulation)
      {
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        float tower_phi = geomIH->get_tower_geometry(key)->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      // if a tower is masked, leave it at zero
      if (tower->get_isHot() || tower->get_isNoCalib() || tower->get_isNotInstr() || tower->get_isBadChi2())
      {
        new_energy = 0;
      }

      ihcal_towerinfos->get_tower_at_channel(channel)->set_time(tower->get_time());
      ihcal_towerinfos->get_tower_at_channel(channel)->set_energy(new_energy);
      if (Verbosity() > 5)
      {
        std::cout << "SubtractTowers::process_event : IHCal tower at ieta / iphi = " << ieta << " / " << iphi << ", pre-sub / after-sub E = " << raw_energy << " / " << new_energy << std::endl;
      }
    }
  }
  else
  {
    RawTowerContainer::ConstRange begin_end_IH = towersIH3->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end_IH.first; rtiter != begin_end_IH.second; ++rtiter)
    {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());

      int this_etabin = geomIH->get_etabin(tower_geom->get_eta());
      int this_phibin = geomIH->get_phibin(tower_geom->get_phi());
      float this_E = tower->get_energy();

      RawTower *new_tower = new RawTowerv1();
      new_tower->set_energy(this_E);
      ihcal_towers->AddTower(this_etabin, this_phibin, new_tower);
    }

    // now fill in additional towers with zero energy to fill out the full grid
    for (int eta = 0; eta < geomIH->get_etabins(); eta++)
    {
      for (int phi = 0; phi < geomIH->get_phibins(); phi++)
      {
        if (!ihcal_towers->getTower(eta, phi))
        {
          RawTower *new_tower = new RawTowerv1();
          new_tower->set_energy(0);
          ihcal_towers->AddTower(eta, phi, new_tower);
        }
      }
    }

    // update towers for background subtraction...
    for (RawTowerContainer::ConstIterator rtiter = ihcal_towers->getTowers().first; rtiter != ihcal_towers->getTowers().second; ++rtiter)
    {
      RawTower *tower = rtiter->second;
      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(1).at(tower->get_bineta());
      if (_use_flow_modulation)
      {
        float tower_phi = geomIH->get_tower_geometry(tower->get_key())->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      if (raw_energy == 0)
      {
        new_energy = 0;
      }
      tower->set_energy(new_energy);
    }
  }
  // OHCal

  // replicate existing towers
  if (m_use_towerinfo)
  {
    unsigned int nchannels_oh = towerinfosOH3->size();
    for (unsigned int channel = 0; channel < nchannels_oh; channel++)
    {
      TowerInfo *tower = towerinfosOH3->get_tower_at_channel(channel);
      unsigned int towerkey = towerinfosOH3->encode_key(channel);
      int ieta = towerinfosOH3->getTowerEtaBin(towerkey);
      int iphi = towerinfosOH3->getTowerPhiBin(towerkey);
      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(2).at(ieta);
      if (_use_flow_modulation)
      {
        const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
        float tower_phi = geomOH->get_tower_geometry(key)->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      // if a tower is masked, leave it at zero
      if (tower->get_isHot() || tower->get_isNoCalib() || tower->get_isNotInstr() || tower->get_isBadChi2())
      {
        new_energy = 0;
      }

      ohcal_towerinfos->get_tower_at_channel(channel)->set_time(tower->get_time());
      ohcal_towerinfos->get_tower_at_channel(channel)->set_energy(new_energy);
      if (Verbosity() > 5)
      {
        std::cout << "SubtractTowers::process_event : OHCal tower at ieta / iphi = " << ieta << " / " << iphi << ", pre-sub / after-sub E = " << raw_energy << " / " << new_energy << std::endl;
      }
    }
  }
  else
  {
    RawTowerContainer::ConstRange begin_end_OH = towersOH3->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end_OH.first; rtiter != begin_end_OH.second; ++rtiter)
    {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());

      int this_etabin = geomOH->get_etabin(tower_geom->get_eta());
      int this_phibin = geomOH->get_phibin(tower_geom->get_phi());
      float this_E = tower->get_energy();

      RawTower *new_tower = new RawTowerv1();
      new_tower->set_energy(this_E);
      ohcal_towers->AddTower(this_etabin, this_phibin, new_tower);
    }

    // now fill in additional towers with zero energy to fill out the full grid
    for (int eta = 0; eta < geomOH->get_etabins(); eta++)
    {
      for (int phi = 0; phi < geomOH->get_phibins(); phi++)
      {
        if (!ohcal_towers->getTower(eta, phi))
        {
          RawTower *new_tower = new RawTowerv1();
          new_tower->set_energy(0);
          ohcal_towers->AddTower(eta, phi, new_tower);
        }
      }
    }

    // update towers for background subtraction...
    for (RawTowerContainer::ConstIterator rtiter = ohcal_towers->getTowers().first; rtiter != ohcal_towers->getTowers().second; ++rtiter)
    {
      RawTower *tower = rtiter->second;
      float raw_energy = tower->get_energy();
      float UE = towerbackground->get_UE(2).at(tower->get_bineta());
      if (_use_flow_modulation)
      {
        float tower_phi = geomOH->get_tower_geometry(tower->get_key())->get_phi();
        UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
      }
      float new_energy = raw_energy - UE;
      if (raw_energy == 0)
      {
        new_energy = 0;
      }
      tower->set_energy(new_energy);
    }
  }

  if (Verbosity() > 0)
  {
    if (!m_use_towerinfo)
    {
      std::cout << "SubtractTowers::process_event: ending with " << emcal_towers->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: ending with " << ihcal_towers->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: ending with " << ohcal_towers->size() << " TOWER_CALIB_HCALOUT_SUB1 towers" << std::endl;
    }
    else
    {
      std::cout << "SubtractTowers::process_event: ending with " << emcal_towerinfos->size() << m_towerNodePrefix << "_CEMC_RETOWER_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: ending with " << ihcal_towerinfos->size() << m_towerNodePrefix << "_HCALIN_SUB1 towers" << std::endl;
      std::cout << "SubtractTowers::process_event: ending with " << ohcal_towerinfos->size() << m_towerNodePrefix << "_HCALOUT_SUB1 towers" << std::endl;
    }
  }

  if (Verbosity() > 0)
  {
    std::cout << "SubtractTowers::process_event: exiting" << std::endl;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int SubtractTowers::CreateNode(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  IHTowerName = m_towerNodePrefix + "_HCALIN";
  TowerInfoContainer *hcal_towers = findNode::getClass<TowerInfoContainer>(topNode, IHTowerName);
  if (m_use_towerinfo && !hcal_towers)
  {
    std::cout << PHWHERE << "Cannot find " << IHTowerName << " for creating new tower containers. Exiting" << std::endl;
    exit(1);
  }

  // store the new EMCal towers

  PHCompositeNode *emcalNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "CEMC"));
  if (!emcalNode)
  {
    std::cout << PHWHERE << "EMCal Node note found, doing nothing." << std::endl;
  }
  if (m_use_towerinfo)
  {
    EMTowerName = m_towerNodePrefix + "_CEMC_RETOWER_SUB1";
    TowerInfoContainer *test_emcal_tower = findNode::getClass<TowerInfoContainer>(topNode, EMTowerName);
    if (!test_emcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating " << EMTowerName << " node " << std::endl;
      }

      TowerInfoContainer *emcal_towers = dynamic_cast<TowerInfoContainer *>(hcal_towers->CloneMe());
      PHIODataNode<PHObject> *emcalTowerNode = new PHIODataNode<PHObject>(emcal_towers, EMTowerName, "PHObject");
      emcalNode->addNode(emcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : " << EMTowerName << " already exists! " << std::endl;
    }
  }
  else
  {
    RawTowerContainer *test_emcal_tower = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
    if (!test_emcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating TOWER_CALIB_CEMC_RETOWER_SUB1 node " << std::endl;
      }

      RawTowerContainer *emcal_towers = new RawTowerContainer(RawTowerDefs::CalorimeterId::HCALIN);
      PHIODataNode<PHObject> *emcalTowerNode = new PHIODataNode<PHObject>(emcal_towers, "TOWER_CALIB_CEMC_RETOWER_SUB1", "PHObject");
      emcalNode->addNode(emcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : TOWER_CALIB_CEMC_RETOWER_SUB1 already exists! " << std::endl;
    }
  }

  // store the new IHCal towers
  PHCompositeNode *ihcalNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "HCALIN"));
  if (!ihcalNode)
  {
    std::cout << PHWHERE << "IHCal Node note found, doing nothing." << std::endl;
  }
  if (m_use_towerinfo)
  {
    IHTowerName = m_towerNodePrefix + "_HCALIN_SUB1";
    TowerInfoContainer *test_ihcal_tower = findNode::getClass<TowerInfoContainer>(topNode, IHTowerName);
    if (!test_ihcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating " << IHTowerName << " node " << std::endl;
      }

      TowerInfoContainer *ihcal_towers = dynamic_cast<TowerInfoContainer *>(hcal_towers->CloneMe());
      PHIODataNode<PHObject> *ihcalTowerNode = new PHIODataNode<PHObject>(ihcal_towers, IHTowerName, "PHObject");
      ihcalNode->addNode(ihcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : " << IHTowerName << " already exists! " << std::endl;
    }
  }
  else
  {
    RawTowerContainer *test_ihcal_tower = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
    if (!test_ihcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating TOWER_CALIB_HCALIN_SUB1 node " << std::endl;
      }

      RawTowerContainer *ihcal_towers = new RawTowerContainer(RawTowerDefs::CalorimeterId::HCALIN);
      PHIODataNode<PHObject> *ihcalTowerNode = new PHIODataNode<PHObject>(ihcal_towers, "TOWER_CALIB_HCALIN_SUB1", "PHObject");
      ihcalNode->addNode(ihcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : TOWER_CALIB_HCALIN_SUB1 already exists! " << std::endl;
    }
  }

  // store the new OHCal towers
  PHCompositeNode *ohcalNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "HCALOUT"));
  if (!ohcalNode)
  {
    std::cout << PHWHERE << "OHCal Node note found, doing nothing." << std::endl;
  }
  if (m_use_towerinfo)
  {
    OHTowerName = m_towerNodePrefix + "_HCALOUT_SUB1";
    TowerInfoContainer *test_ohcal_tower = findNode::getClass<TowerInfoContainer>(topNode, OHTowerName);
    if (!test_ohcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating " << OHTowerName << " node " << std::endl;
      }

      TowerInfoContainer *ohcal_towers = dynamic_cast<TowerInfoContainer *>(hcal_towers->CloneMe());
      PHIODataNode<PHObject> *ohcalTowerNode = new PHIODataNode<PHObject>(ohcal_towers, OHTowerName, "PHObject");
      ohcalNode->addNode(ohcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : " << OHTowerName << " already exists! " << std::endl;
    }
  }
  else
  {
    RawTowerContainer *test_ohcal_tower = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
    if (!test_ohcal_tower)
    {
      if (Verbosity() > 0)
      {
        std::cout << "SubtractTowers::CreateNode : creating TOWER_CALIB_HCALOUT_SUB1 node " << std::endl;
      }

      RawTowerContainer *ohcal_towers = new RawTowerContainer(RawTowerDefs::CalorimeterId::HCALOUT);
      PHIODataNode<PHObject> *ohcalTowerNode = new PHIODataNode<PHObject>(ohcal_towers, "TOWER_CALIB_HCALOUT_SUB1", "PHObject");
      ohcalNode->addNode(ohcalTowerNode);
    }
    else
    {
      std::cout << "SubtractTowers::CreateNode : TOWER_CALIB_HCALOUT_SUB1 already exists! " << std::endl;
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}
