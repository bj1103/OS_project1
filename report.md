# report

B07902114 陳柏衡

## 1. 設計

宣告一個陣列event，它會紀錄從main process開始到最後一個process結束之間，按照時間順序發生的所有event (cpu閒置幾秒， 某一個process要被fork出來，某一個process run了幾秒)；然後依照所選的cpu scheduling方式(FIFO, RR, SJF, PSJF)，將每一筆test對應該scheduling方式會發生的所有event，按照時間順序紀錄在event陣列中，接著再使cpu依照event陣列在每個時間點去執行相對的event，即完成排程。

## 2 . 核心版本

4.14.25

## 3. 比較

理論上，兩個都執行Y units of time的process，在兩個process都沒有被preemptive的情況下，兩個process在cpu執行的時間應該要是完全相同的，但在實作中，因為我是採取只用一顆cpu的做法，因此即便某process P1沒有被preemptive，但可能在其執行期間中，有另一process P2抵達必須立即被fork，則必須先從P1 context switch回main process，fork出P2後，再context switch回到P1，因此，即使沒有preemptive，process的finish time - start time也不一定完全是該process在cpu的時間，而是還包含了期間context switch回main process並fork其他process的時間；此外，因為我們實做的是user-space scheduler，因此即便我們的code schedule到某一process，並使其執行Y units of time，但在Y units of time期間，kernel仍然可能context switch去做其他事情，因此最後我們得到的finish time - start time也就不會恰好是該process所執行的Y units of time，上述兩個狀況就造成了實作與理論上的不同。