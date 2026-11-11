import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '../views/HomeView.vue'
import Member from '../views/Member.vue'
import BorrowersView from '../views/BorrowersView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView
    },
    {
      path: '/borrowers',
      name: 'borrowers',
      component: BorrowersView
    },
    {
      path: '/member',
      name: 'member',
      component: Member
    }
  ]
})

export default router