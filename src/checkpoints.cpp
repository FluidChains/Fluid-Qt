// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  0,     uint256("0x000007c70a931f5cafa42d48eb0f7a627007cada84d84442e7369cc962a0d38f") )
        (  2,     uint256("0x3fa697d0ba0e948bcd268f2fc316d2c7b41904eeb1bced590053ea0f406cf014") ) // Premine
        ( 50,     uint256("0x5ba5192b53f4c23238e18835195abddbd1e1b6c7ecb320371a83ee57ba43d8cc") )
        ( 100,    uint256("0x4de52b8d1803c0672cf012e9087e967001b0d1912299abed8dc5c90ba2ff5dba") )
        ( 500,    uint256("0xc8f2c0bfcea07a71a9d3b5b968995b03bd507f21f2a1697790f0ab421387a500") )
        ( 1000,   uint256("0x93a5c8c3f0f1cfb75122cf02b4413f642b5870ede34689c9e3adf2aa8cb05918") )
        ( 4000,   uint256("0x476fac3dc504ad4c154adce532bdfce9b136dc37c29f04141cc16d3290579223") )
        ( 6000,   uint256("0x3750a9ad41a606254923b2f7ad0f22e82864623e65160a7160f270a4c991201c") )
        ( 6002,   uint256("0x9f4aae7b2c472741dd72a964c6ff680aaafcf2198e651eae665a756f280532b7") )
        ( 10000,  uint256("0x7bf91167a8fbcbd25f28025769f106ad6bc49d0407c971d0841e58528b65aa92") )
        ( 15000,  uint256("0x43288ba814b30026d2acdc67064c4ae5f0e54a3463961aae84dd25025bbbb780") ) // v1.0.0.1
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
